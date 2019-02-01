import React from 'react';
import PropTypes from 'prop-types';

import {
  View,
  Text,
  TouchableOpacity,
  StyleSheet
} from 'react-native';

import {
  COMMAND_TYPE
} from '../constant/enum';

/**
 * Small control cluster housing actions outside of core product
 */
export class DemoControls extends React.Component {

  static propTypes = {
    /**
     * Instance of network/TCPSocket
     */
    dispatchSocket: PropTypes.object,
    /**
     * Object from constant/theme
     */
    theme: PropTypes.object
  }

  constructor(props) {
    super(props);
  }

  onPrintBlockchainPress = () => {
    this.props.dispatchSocket.write({
      type: COMMAND_TYPE.demo,
      payload: {
        type: 'blockchain'
      }
    });
  }

  onRejectedTransactionPress = () => {
    this.props.dispatchSocket.write({
      type: COMMAND_TYPE.node,
      payload: {
        room: 'kitchen',
        node: 'stove',
        value: 1
      }
    });
  }
  
  renderButton = (title, onPress, color) => {
    return (
      <TouchableOpacity
        activeOpacity={0.6}
        onPress={onPress}
      >
        <Text style={{
          color
        }}
        >
          {title}
        </Text>
      </TouchableOpacity>
    );
  }

  render() {
    return (
      <View style={styles.controlContainer}>
        {this.renderButton('Print blockchain', this.onPrintBlockchainPress, this.props.theme.primaryText)}
        {this.renderButton('Unauthorized transaction', this.onRejectedTransactionPress, 'red')}
      </View>
    );
  }
}

const styles = StyleSheet.create({
  controlContainer: {
    flex: 1,
    paddingHorizontal: 10,
    paddingVertical: 5,
    alignItems: 'center',
    flexDirection: 'row',
    justifyContent: 'space-around'
  }
});
