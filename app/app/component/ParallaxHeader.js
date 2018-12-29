/*
  Original credit to Montie Thakkar
  https://github.com/Monte9/react-native-parallax-scrollview
  MIT Licensed
  Heavily modified implementation of a parallax ScrollView
*/

import React from 'react';

import {
  StyleSheet,
  Platform,
  Animated,
  Dimensions,
  View,
  Text,
  Image,
  StatusBar
} from 'react-native';

import LinearGradient from 'react-native-linear-gradient';

const {
  height: SCREEN_HEIGHT,
} = Dimensions.get('window');

const IS_IPHONE_X = SCREEN_HEIGHT === 812;

const SCROLL_EVENT_THROTTLE = 2;
const DEFAULT_HEADER_MAX_HEIGHT = 240;
const DEFAULT_HEADER_MIN_HEIGHT = 64;
const DEFAULT_EXTRA_SCROLL_HEIGHT = 50;
const DEFAULT_BACKGROUND_IMAGE_SCALE = 1.5;
const DEFAULT_BACKGROUND_COLOR = '#fff'

const hit_slop = {top: 44, bottom: 44, left: 44, right: 44}

const styles = StyleSheet.create({
  container: {
    backgroundColor: '#f2f2f2',
    flex: 1,
    height: 50
  },
  header: {
    position: 'absolute',
    top: 0,
    left: 0,
    right: 0,
    overflow: 'hidden',
  },
  backgroundImage: {
    position: 'absolute',
    top: 0,
    left: 0,
    right: 0,
    width: 300,
    height: 200,
    resizeMode: 'cover',
    backgroundColor: 'transparent'
  },
  bar: {
    height: DEFAULT_HEADER_MIN_HEIGHT,
    position: 'absolute',
    top: 0,
    left: 0,
    right: 0
  },
  headerTitle: {
    backgroundColor: 'transparent',
    position: 'absolute',
    top: 0,
    left: 0,
    right: 0,
    alignItems: 'center',
    justifyContent: 'center'
  },
  headerText: {
    textAlign: 'center',
    fontSize: 16,
    fontWeight: 'bold',

    textShadowOffset: {width: -1, height: 2},
    textShadowColor: 'rgba(0, 0, 0, 0.3)',
    textShadowRadius: 4,
  },
  
  leftContainer: {
    flex: 1,
    flexDirection: 'row',
    justifyContent: 'flex-start',
  },
  rightContainer: {
    flex: 1,
    flexDirection: 'row',
    justifyContent: 'flex-end',
    alignItems: 'center',
  },
  separator: {
    paddingTop: 15,
    alignItems: 'center',
    justifyContent: 'space-between',
  },
  ico: {
    fontSize: 22,
    backgroundColor: 'transparent',
  },
  icoContain: {
    width: 100,
    height: 100
  }
});

const Animg = Animated.createAnimatedComponent(Image);

export class ParallaxHeader extends React.Component {
  static defaultProps = {
    renderNavBar: () => <View />,
    backgroundColor: DEFAULT_BACKGROUND_COLOR,
    backgroundImage: null,
    headerMaxHeight: DEFAULT_HEADER_MAX_HEIGHT,
    headerMinHeight: DEFAULT_HEADER_MIN_HEIGHT,
    scrollEventThrottle: SCROLL_EVENT_THROTTLE,
    extraScrollHeight: DEFAULT_EXTRA_SCROLL_HEIGHT,
    backgroundImageScale: DEFAULT_BACKGROUND_IMAGE_SCALE,
  }

  constructor() {
    super();
    
    this.state = {
      scrollY: new Animated.Value(0),
    };
  }

  componentDidMount() {
    this.state.scrollY.addListener(this.assessScrollY)
  }

  assessScrollY = ({value}) => {
    if (value < 10) {
      StatusBar.setBarStyle('light-content', true);
    } else if (value > this.props.headerMinHeight) {
      StatusBar.setBarStyle('default', true);
    }
  }

  getHeaderMaxHeight() {
    const { headerMaxHeight } = this.props;
    return headerMaxHeight;
  }

  getHeaderMinHeight() {
    const { headerMinHeight } = this.props;
    return headerMinHeight;
  }

  getHeaderScrollDistance() {
    return this.getHeaderMaxHeight() - this.getHeaderMinHeight();
  }

  getExtraScrollHeight() {
    const { extraScrollHeight } = this.props;
    return extraScrollHeight;
  }

  getBackgroundImageScale() {
    const { backgroundImageScale } = this.props;
    return backgroundImageScale;
  }

  getInputRange() {
    return [-this.getExtraScrollHeight(), 0,  this.getHeaderScrollDistance()];
  }

  getHeaderHeight() {
    return this.state.scrollY.interpolate({
      inputRange: this.getInputRange(),
      outputRange: [this.getHeaderMaxHeight() + this.getExtraScrollHeight(), this.getHeaderMaxHeight(), this.getHeaderMinHeight()],
      extrapolate: 'clamp',
    });
  }

  getNavBarOpacity() {
    return this.state.scrollY.interpolate({
      inputRange: this.getInputRange(),
      outputRange: [0, 0, 1],
      extrapolate: 'clamp',
    });
  }

  getImageOpacity() {
    return this.state.scrollY.interpolate({
      inputRange: this.getInputRange(),
      outputRange: [1, 1, 0],
      extrapolate: 'clamp',
    });
  }

  getImageTranslate() {
    return this.state.scrollY.interpolate({
      inputRange: this.getInputRange(),
      outputRange: [0, 0, -50],
      extrapolate: 'clamp',
    });
  }

  getImageScale() {
    return this.state.scrollY.interpolate({
      inputRange: this.getInputRange(),
      outputRange: [this.getBackgroundImageScale(), 1, 1],
      extrapolate: 'clamp'
    });
  }

  getTitleTranslate() {
    return this.state.scrollY.interpolate({
      inputRange: this.getInputRange(),
      outputRange: [5, 0, 0],
      extrapolate: 'clamp',
    });
  }

  getColorChange() {
    return this.state.scrollY.interpolate({
      inputRange: [0, 200],
      outputRange: ['rgba(255, 255, 255, 1)', 'rgba(0, 0, 0, 1)']
    })
  }

  renderHeaderTitle() {
    return (
      <Animated.View
        style={[
          styles.headerTitle, {
            transform: [{
              translateY: this.getTitleTranslate()
            }],

            height: this.getHeaderHeight(),
          },
        ]}
      >
        <Animated.Text
          allowFontScaling={false}
          style={[styles.headerText, this.props.titleStyle, {
            color: this.getColorChange(),

            fontSize: this.state.scrollY.interpolate({
              inputRange: [0, 40],
              outputRange: [this.props.titleMaxFontSize || 30, this.props.titleMaxFontSize - this.props.minFontReduction],
              extrapolate: 'clamp',
            })
          }]}
        >
          {this.props.title}
        </Animated.Text>
      </Animated.View>
    );
  }

  renderHeaderForeground() {
    return (
        <Animated.View
          style={[styles.bar, {
            height: this.getHeaderMinHeight(),
          }]}
        />
    );
  }

  renderBackgroundImage() {
    return (
      <Animg
        source={this.props.backgroundImage}
        style={[styles.backgroundImage, {
          resizeMode: 'cover',
          height: this.getHeaderMaxHeight(),
          width: '100%',
            transform: [{
              translateY: this.getImageTranslate()
            }, {
              scale:  this.getImageScale()
            }],
        }]}

        key={0}
        animationDuration={200}
        useNativeDriver
      />
    );
  }

  renderPlainBackground() {
    const { backgroundColor } = this.props;
    
    const imageOpacity = this.getImageOpacity();
    const imageTranslate = this.getImageTranslate();
    const imageScale = this.getImageScale();
    
    return (
      <Animated.View useNativeDriver
        style={{
          height: this.getHeaderMaxHeight(),
          backgroundColor,
          opacity: imageOpacity,
          transform: [{
            translateY: imageTranslate,
            scale: imageScale
          }]
        }}
      />
    );
  }

  renderHeaderBackground() {
    const { backgroundImage } = this.props;    
    
    return (
      <Animated.View useNativeDriver
        style={[
          styles.header,
          {
            height: this.getHeaderHeight(),
            opacity: this.getImageOpacity(),
            backgroundColor: 'transparent'
          },
        ]}
      >
        {backgroundImage ? this.renderBackgroundImage() : this.renderPlainBackground()}
      </Animated.View>
    );
  }

  render() {
    return (
      <LinearGradient style={styles.container} colors={this.props.backgroundGradientColors}>
        <Animated.ScrollView
          scrollEventThrottle={this.props.scrollEventThrottle}
          scrollEnabled={this.props.scrollEnabled}
          onScroll={Animated.event([{
            nativeEvent: {
              contentOffset: {
                y: this.state.scrollY
                }
              }
            }]
          )}
        >
          <View style={{ marginTop: this.getHeaderMaxHeight() }}>
            {this.props.children}
          </View>
        </Animated.ScrollView>
        
        <Animated.View //navbar background
          style={[styles.header, {
              height: this.getHeaderHeight(),
              backgroundColor: '#f4f4f4',
              opacity: this.getNavBarOpacity(),
            }
          ]}
        />
        {this.renderHeaderBackground()}
        {this.renderHeaderTitle()}
        {this.renderHeaderForeground()}
      </LinearGradient>
    );
  }
}
