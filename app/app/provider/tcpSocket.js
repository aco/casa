import { 
  NativeModules,
  NativeEventEmitter
} from 'react-native';

/**
 * Native iOS bridge to an async TCP socket
 */
export class TCPSocket {

  constructor(host, port) {
    this._observers = [];

    this.dispatchSocket = NativeModules.TCPSocket;
    this._eventEmitter = new NativeEventEmitter(NativeModules.TCPSocket);

    this.onConnect = undefined;
    this.onData = undefined;
    this.onClose = undefined;
    this.onError = undefined;

    this._registerObservers();

    this.dispatchSocket.createConnection(host, 8036, false);
  }

  _registerObservers = () => {
    if (this._observers.length > 0) {
      return;
    }

    this._observers = [
      this._eventEmitter.addListener('connect', this._onConnect),
      this._eventEmitter.addListener('data', this._onData),
      this._eventEmitter.addListener('close', this._onClose),
      this._eventEmitter.addListener('error', this._onError)
    ];
  }

  _onConnect = (address) => {
    console.log('received', 'connect', address);
  }

  _onData = (data) => {
    this.onData(data);
  }

  _onClose = (hadError) => {
    console.log('received', 'close', hadError);
  }

  _onError = (error) => {
    console.log('received', 'error', error);

    // this.emit('error', normalizeError(error));
    this.destroy();
  }

  write = (data) => {
    this.dispatchSocket.writeString(JSON.stringify(data), -1);
  }

  destroy = () => {
    this._observers.forEach(e => e.remove());
    this._observers = [];
  }
}
