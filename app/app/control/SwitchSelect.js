/*
  Original credit to App2Sales
  https://github.com/App2Sales/react-native-switch-selector
  MIT Licensed
  Modified implementation
*/

import React from 'react';

import {
  View,
  Text,
  Animated,
  TouchableOpacity,
  Easing,
  PanResponder
} from 'react-native';

const styles = {
  button: {
    flexDirection: 'row',
    justifyContent: 'center',
    alignItems: 'center'
  },
  containerButton: {
    flexDirection: 'row',
    flex: 1,
    height: 40,
    justifyContent: 'center',
    alignItems: 'center'
  },
  animated: {
    borderRadius: 50,
    borderWidth: 0,
    position: 'absolute'
  }
};

const hit_slop = {
  top: 44,
  left: 10,
  right: 10,
  bottom: 44
}

export class SwitchSelect extends React.Component {
  constructor (props) {
    super(props);
    this.state = {
      selected: this.props.initial ? this.props.initial : 0
    };
    this.animatedValue = new Animated.Value(
      this.props.initial ? this.props.initial / this.props.options.length : 0
    );
  }

  componentWillMount () {
    this._panResponder = PanResponder.create({
      onStartShouldSetPanResponder: this.shouldSetResponder,
      onMoveShouldSetPanResponder: this.shouldSetResponder,
      onPanResponderRelease: this.responderEnd,
      onPanResponderTerminate: this.responderEnd
    });
  }

  componentWillReceiveProps (nextProps) {
    if (nextProps.value !== this.props.value) {
      this.toggleItem(nextProps.value);
    }
  }

  shouldSetResponder = (evt, gestureState) => {
    return (
      evt.nativeEvent.touches.length === 1 &&
      !(Math.abs(gestureState.dx) < 5 && Math.abs(gestureState.dy) < 5)
    );
  };

  responderEnd = (evt, gestureState) => {
    const swipeDirection = this._getSwipeDirection(gestureState);
    if (
      swipeDirection === 'RIGHT' &&
      this.state.selected < this.props.options.length - 1
    ) {
      this.toggleItem(this.state.selected + 1);
    } else if (swipeDirection === 'LEFT' && this.state.selected > 0) {
      this.toggleItem(this.state.selected - 1);
    }
  };

  _getSwipeDirection (gestureState) {
    const { dx, dy, vx } = gestureState;
    // 0.1 velocity
    if (Math.abs(vx) > 0.1 && Math.abs(dy) < 80) {
      return dx > 0 ? 'RIGHT' : 'LEFT';
    }
    return null;
  }

  getBgColor () {
    return this.props.buttonColor;
  }

  animate = (value, last) => {
    this.animatedValue.setValue(last);

    Animated.timing(this.animatedValue, {
      toValue: value,
      duration: this.props.animationDuration,
      easing: Easing.inOut(Easing.exp),
      useNativeDriver: true,
    }).start();
  };

  toggleItem = index => {
    const { options, returnObject, onPress } = this.props;
    if (options.length <= 1) return;
    this.animate(
      index / options.length,
      this.state.selected / options.length
    );
    if (onPress) {
      onPress(returnObject ? options[index] : options[index].value);
    } else {
      console.log('Call onPress with value: ', options[index].value);
    }
    this.setState({ selected: index });
  };

  render () {
    const {
      style,
      textStyle,
      selectedTextStyle,
      textColor,
      selectedColor,
      fontSize,
      backgroundColor,
      borderColor,
      hasPadding,
      valuePadding,
      height,
      bold
    } = this.props;

    const options = this.props.options.map((element, index) => (
      <View
        key={index}
        style={{ flex: 1, flexDirection: 'column', justifyContent: 'center' }}>
        <TouchableOpacity
          style={styles.button}
          hitSlop={hit_slop}
          onPress={() => this.toggleItem(index)}>
          <Text
            allowFontScaling={false}
            style={[
              {
                fontSize,
                fontWeight: bold ? 'bold' : 'normal',
                textAlign: 'center',
                color: this.state.selected == index ? selectedColor : textColor,
                backgroundColor: 'transparent'
              },
              this.state.selected == index ? selectedTextStyle : textStyle
            ]}
          >
            {element.label}
          </Text>
        </TouchableOpacity>
      </View>
    ));

    return (
      <View style={[{ flexDirection: 'row' }, style]}>
        <View {...this._panResponder.panHandlers} style={{ flex: 1 }}>
          <View
            style={{
              borderRadius: 50,
              backgroundColor: backgroundColor,
              height
            }}
            onLayout={event => {
              const { width } = event.nativeEvent.layout;
              this.setState({
                sliderWidth: width - (hasPadding ? valuePadding : 0),
              });
            }}>
            <View
              style={{
                flex: 1,
                flexDirection: 'row',
                borderColor: this.props.borderColor,
                borderRadius: 60,
                borderWidth: hasPadding ? 1 : 0
              }}>
              {this.state.sliderWidth &&
                <Animated.View
                  style={[
                    {
                      height: hasPadding ? height - 4 : height,
                      backgroundColor: this.getBgColor(),
                      width: this.state.sliderWidth /
                        this.props.options.length -
                        (hasPadding ? valuePadding : 0),
                      transform: [
                        {
                          translateX: this.animatedValue.interpolate({
                            inputRange: [0, 1],
                            outputRange: [
                              hasPadding ? valuePadding : 0,
                              this.state.sliderWidth -
                              (hasPadding ? valuePadding : 0)
                            ],
                          }),
                        },
                      ],
                      marginTop: hasPadding ? valuePadding : 0
                    },
                    styles.animated
                  ]}
                />}
              {options}
            </View>
          </View>
        </View>
      </View>
    );
  }
}

SwitchSelect.defaultProps = {
  style: {},
  textStyle: {},
  selectedTextStyle: {},
  textColor: '#000000',
  selectedColor: '#FFFFFF',
  fontSize: 15,
  backgroundColor: 'transparent',
  hasPadding: false,
  valuePadding: 1,
  height: 40,
  bold: false,
  buttonColor: '#BCD635',
  returnObject: false,
  animationDuration: 400
};