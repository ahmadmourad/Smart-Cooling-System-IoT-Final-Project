import 'dart:io';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';

class MQTTClientWrapper {
  late MqttServerClient client;
  Function(String)? onMessageReceived;

  MqttCurrentConnectionState connectionState = MqttCurrentConnectionState.IDLE;
  MqttSubscriptionState subscriptionState = MqttSubscriptionState.IDLE;

  void prepareMqttClient() async {
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

  Future<void> _connectClient() async {
    try {
      print('client connecting....');
      connectionState = MqttCurrentConnectionState.CONNECTING;
      await client.connect(
          'hivemq.webclient.1724501117674', 'edD43PXh?0FZ;fY.p&7n');
    } on Exception catch (e) {
      print('client exception - $e');
      connectionState = MqttCurrentConnectionState.ERROR_WHEN_CONNECTING;
      client.disconnect();
    }

    if (client.connectionStatus?.state == MqttConnectionState.connected) {
      connectionState = MqttCurrentConnectionState.CONNECTED;
      print('client connected');
    } else {
      print(
          'ERROR client connection failed - disconnecting, status is ${client.connectionStatus}');
      connectionState = MqttCurrentConnectionState.ERROR_WHEN_CONNECTING;
      client.disconnect();
    }
  }

  void _setupMqttClient() {
    client = MqttServerClient.withPort(
        'b1a69793bf6e48eca7ab350e88d63dd4.s1.eu.hivemq.cloud',
        'hivemq.webclient.1724501117674',
        8883);
    client.secure = true;
    client.securityContext = SecurityContext.defaultContext;
    client.keepAlivePeriod = 20;
    client.onDisconnected = _onDisconnected;
    client.onConnected = _onConnected;
    client.onSubscribed = _onSubscribed;
  }

  void subscribeToTopic(String topicName) {
    print('Subscribing to the $topicName topic');
    client.subscribe(topicName, MqttQos.atMostOnce);
    client.updates?.listen((List<MqttReceivedMessage<MqttMessage>> c) {
      final MqttPublishMessage recMess = c[0].payload as MqttPublishMessage;
      var message =
          MqttPublishPayload.bytesToStringAsString(recMess.payload.message);

      if (onMessageReceived != null) {
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
