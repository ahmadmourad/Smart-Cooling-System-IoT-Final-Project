import 'dart:io';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';

class MQTTClientWrapper {
  late MqttServerClient client; // MqttServerClient instance
  Function(String)? onMessageReceived;

  MqttCurrentConnectionState connectionState = MqttCurrentConnectionState
      .IDLE; // MqttCurrentConnectionState initially IDLE
  MqttSubscriptionState subscriptionState =
      MqttSubscriptionState.IDLE; // MqttSubscriptionState initially IDLE

  // Prepares the MQTT client for use by setting up the client and connecting it to the MQTT broker.
  // After connecting, it subscribes to the topics that are required by the ESP32.

  // This method is asynchronous and returns a Future that completes when the client is connected and subscribed to all topics.
  Future<void> prepareMqttClient() async {
    _setupMqttClient();
    await _connectClient();
    subscribeToTopic('esp32/temperatureSensor');
    subscribeToTopic('esp32/irSensor');
    subscribeToTopic('esp32/flameSensor');
    subscribeToTopic('esp32/gasSensor');
    subscribeToTopic('esp32/smokeSensor');
    subscribeToTopic('esp32/humiditySensor');
    subscribeToTopic('esp32/fan1');
    subscribeToTopic('esp32/fan2');
  }

  // Connects the MQTT client to the MQTT broker.

  // Future is returned after the client has successfully connected to the MQTT broker.
  Future<void> _connectClient() async {
    try {
      print('Connecting to MQTT broker....');
      connectionState = MqttCurrentConnectionState
          .CONNECTING; // Set the connection state to CONNECTING

      // Connect to the MQTT broker with the specified username and password
      await client.connect('AhmedMourad', '0XgUj^W11');
    } on Exception catch (e) {
      print('Error connecting to MQTT broker - $e');
      connectionState = MqttCurrentConnectionState
          .ERROR_WHEN_CONNECTING; // Set the connection state to ERROR_WHEN_CONNECTING
      client.disconnect();
    }

    // Check the connection status
    if (client.connectionStatus?.state == MqttConnectionState.connected) {
      connectionState = MqttCurrentConnectionState.CONNECTED;
      print('Connected to MQTT broker');
    } else {
      print(
          'ERROR connecting to MQTT broker failed - disconnecting, status is ${client.connectionStatus}');
      connectionState = MqttCurrentConnectionState.ERROR_WHEN_CONNECTING;
      client.disconnect();
    }
  }

  /// Sets up the MQTT client with the server, port and credentials.
  void _setupMqttClient() {
    client = MqttServerClient.withPort(
        'e56327f5fee34d15bf973e487b0eb75e.s1.eu.hivemq.cloud',
        'AhmedMourad',
        8883);
    client.secure = true;
    client.securityContext = SecurityContext.defaultContext;
    client.keepAlivePeriod = 20;
    client.onDisconnected = _onDisconnected;
    client.onConnected = _onConnected;
    client.onSubscribed = _onSubscribed;
  }

  // Subscribes to the specified MQTT topic.

  // This function subscribes to the specified MQTT topic and listens for
  // incoming messages. When a message is received, the callback is called with the message.
  void subscribeToTopic(String topicName) {
    print('Subscribing to the $topicName topic');
    client.subscribe(topicName, MqttQos.atMostOnce); // Subscribe to the topic
    client.updates?.listen((List<MqttReceivedMessage<MqttMessage>> c) {
      final MqttPublishMessage recMess = c[0].payload as MqttPublishMessage;
      var message =
          MqttPublishPayload.bytesToStringAsString(recMess.payload.message);

      if (onMessageReceived != null) {
        // Call the onMessageReceived callback if it's not null
        onMessageReceived!(message);
      }
    });
    subscriptionState = MqttSubscriptionState.SUBSCRIBED;
  }

  void unsubscribeFromTopic(String topicName) {
    print('Unsubscribing from the $topicName topic');
    client.unsubscribe(topicName);
    subscriptionState = MqttSubscriptionState.UNSUBSCRIBED;
  }

  void _onSubscribed(String topic) {
    print('Subscribed to $topic');
  }

  void _onDisconnected() {
    print('Disconnected');
  }

  void _onConnected() {
    connectionState = MqttCurrentConnectionState.CONNECTED;
    print('Connected');
  }

// Publishing a message to the specified MQTT topic.
  void publishMessage(String topic, String message) {
    final MqttClientPayloadBuilder builder = MqttClientPayloadBuilder();
    builder.addString(message);
    client.publishMessage(topic, MqttQos.exactlyOnce, builder.payload!);
    print('Published message to $topic: $message');
  }
}

enum MqttCurrentConnectionState {
  IDLE,
  CONNECTING,
  CONNECTED,
  ERROR_WHEN_CONNECTING
}

enum MqttSubscriptionState { IDLE, SUBSCRIBED, UNSUBSCRIBED }
