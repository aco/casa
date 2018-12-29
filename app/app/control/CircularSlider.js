/*
  Licensing under default StackExchange CC-BY-SA
  Original credit to Stevie of Stack Overflow
  https://stackoverflow.com/a/39041997/4255788
  Heavily modified circular slider
*/

import React from 'react'

import {
  PanResponder,
  View,
} from 'react-native'

import Svg, {
  Path,
  Circle,
  Rect,
  Text,
  G as Graphic
} from 'react-native-svg'

export class CircularSlider extends React.Component {
  constructor(props) {
    super(props)

    this.handlePanResponderMove = this.handlePanResponderMove.bind(this)
    this.cartesianToPolar = this.cartesianToPolar.bind(this)
    this.polarToCartesian = this.polarToCartesian.bind(this)

    const {
      width,
      height,
      value
    } = props;

    const smallestSide = (Math.min(width, height))

    this.state = {
      cx: width / 2,
      cy: height / 2,
      r: (smallestSide / 2) * 0.85
    };
  }

  componentWillMount() {
    if (!this.props.display) {
      this._panResponder = PanResponder.create({
        onStartShouldSetPanResponder: this.props.onTogglePan,
        onMoveShouldSetPanResponder: () => true,
        onPanResponderMove: this.handlePanResponderMove,
        onPanResponderEnd: this.props.onTogglePan,
        onPanResponderRelease: this.props.onPanRelease
      });
    }
  }

  polarToCartesian(angle) {
    const {
      cx,
      cy,
      r
    } = this.state;
    
    const a = (angle-270) * Math.PI / 180.0,
      x = cx + (r * Math.cos(a)),
      y = cy + (r * Math.sin(a));
    
    return {
      x,
      y
    };
  }

  cartesianToPolar(x,y) {
    const {
      cx,
      cy
    } = this.state;

    return Math.round((Math.atan((y - cy) / (x - cx))) / (Math.PI / 180) + ((x > cx) ? 270 : 90));
  }

  handlePanResponderMove({
    nativeEvent: {
      locationX,
      locationY
    }
  }) {
    const polarized = this.cartesianToPolar(locationX, locationY);
    const rangedValue = ((polarized / 360) * this.props.upperBound);

    this.props.onValueChange(rangedValue);
  }

  render() {
    const {
      width,
      height,
      theme,
      rangedValue,
      textColor,
      descriptionColor,
      meterColor,
      description,
      displayValue
    } = this.props;
    
    const {
      cx,
      cy,
      r,
    } = this.state;

    const value = ((rangedValue) / (this.props.upperBound) * 360);

    const startCoord = this.polarToCartesian(0),
      endCoord = this.polarToCartesian(value);

    return (
      <Svg onLayout={this.onLayout} width={width} height={height} style={this.props.style}>
        <Circle cx={cx} cy={cy} r={r} stroke={this.props.borderColor} strokeWidth={10} fill='none' />
        
        <Text
          key='title'
          x={width / 2}
          y={(height / 2) - 10}
          fontSize={this.props.baseFontSize}
          fill={descriptionColor}
          textAnchor='middle'
          allowFontScaling={false}
        >
          {description}
        </Text>

        <Text
          key={value}
          x={width / 2}
          y={(height / 2) + 15}
          fontSize={this.props.baseFontSize + 4}
          fill={textColor}
          textAnchor='middle'
          allowFontScaling={false}
        >
          {displayValue}
        </Text>

        <Path
          stroke={meterColor}
          strokeWidth={10}
          fill='none'
          d={`M${startCoord.x} ${startCoord.y} A ${r} ${r} 0 ${value > 180 ? 1 : 0} 1 ${endCoord.x} ${endCoord.y}`}
        />

        {!this.props.display &&
        <Graphic
          x={endCoord.x-7.5}
          y={endCoord.y-7.5}
        >
          <Circle
            cx={8}
            cy={8}
            r={10}
            fill={meterColor}
          />

          <Rect
              x={-20}
              y={-20}
              width='50'
              height='50'
              fill='rgba(0, 0, 0, 0.0)'
              {...this._panResponder.panHandlers}
            />
        </Graphic>}
      </Svg>
    )
  }
}
