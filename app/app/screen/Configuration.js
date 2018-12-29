import React from 'react';

import { 
  View,
  Text,
  StyleSheet,
  TextInput,
  TouchableOpacity,
  KeyboardAvoidingView
} from 'react-native';

import THEME from '../constant/theme';

const default_host = '192.168.0.15';
const default_port = 4045;

/**
 * Configuration screen for the host and port of controller - can also run as a demo
 */
export class Configuration extends React.Component {

  constructor(props) {
    super(props);

    this.host = null;
    this.port = null;
  }

  render() {
    return (
      <KeyboardAvoidingView
        style={styles.container}
        behavior='position'
      >
        <TextInput
          style={styles.textInput}
          placeholder={`Host (${default_host})`}
          keyboardType='url'
          placeholderTextColor={THEME.light.tertiaryText}
          onChangeText={text =>
            this.host = text
          }
        />

        <TextInput
          style={styles.textInput}
          placeholder={`Port (${default_port})`}
          keyboardType='numeric'
          placeholderTextColor={THEME.light.tertiaryText}
          onChangeText={text =>
            this.port = parseInt(text)
          }
        />

        <View style={styles.buttonContainer}>
          <TouchableOpacity
            style={[styles.button, styles.buttonPill]}
            activeOpacity={0.6}

            onPress={() =>
              this.props.onSubmitConfiguration({
                host: this.host || default_host,
                port: this.port || default_port
              })
            }
          >
            <Text style={styles.pillButtonLabel}>Connect</Text>
          </TouchableOpacity>

          <TouchableOpacity
            style={styles.button}
            activeOpacity={0.6}

            onPress={() =>
              this.props.onSubmitConfiguration(null)
            }
          >
            <Text style={styles.buttonLabel}>Run as Demo</Text>
          </TouchableOpacity>
        </View>
      </KeyboardAvoidingView>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    paddingHorizontal: 10,
    paddingVertical: 5,
  },
  buttonContainer: {
    alignItems: 'center',
    flex: 1,
    justifyContent: 'space-between'
  },
  button: {
    alignItems: 'center',
    justifyContent: 'center',
    marginTop: 30
  },
  buttonPill: {
    height: 50,
    borderRadius: 25,
    borderLeftColor: 'transparent',
    borderRightColor: 'transparent',
    backgroundColor: THEME.light.accentControl,
  },
  pillButtonLabel: {
    color: 'white',
    fontSize: 15,
    paddingHorizontal: 50
  },
  buttonLabel: {
    color: THEME.light.primaryText,
    fontSize: 14
  },
  textInput: {
    height: 30,
    width: '100%',
    marginTop: 15,
    borderBottomWidth: StyleSheet.hairlineWidth,
    borderBottomColor: THEME.light.accentControl,
    color: THEME.light.primaryText
  }
});
