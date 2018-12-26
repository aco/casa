import React from 'react';
import PropTypes from 'prop-types';

import {
  View,
  Text,
  ScrollView,
  TouchableOpacity,
  StyleSheet,
  LayoutAnimation,
  Dimensions
} from 'react-native';

import {
  titleCase
} from '../util';

import AnimatedEntrance from 'react-native-animated-entrance';

const HIT_SLOPE = {
  top: 44,
  bottom: 44,
  left: 2,
  right: 2
}

const {
  width: SCREEN_WIDTH
} = Dimensions.get('window');

export class TabSlider extends React.Component {

  state = {
    activeTab: 0
  }

  static propTypes = {
    /**
     * Whether the tab pages are user-scrollable
     */
    fixed: PropTypes.bool,
    /**
     * Handler called on active tab change - passes new active tab
     */
    tabIndexChanged: PropTypes.func,
    /**
     * Optional: custom renderer for tab buttons - passes title, currently active tab and index position of tab button
     */
    renderTabButton: PropTypes.func,
    /**
     * Theme object from constant/Theme
     */
    theme: PropTypes.object
  }

  /**
   * Scrolls to new tab, calculates on screen width
   * @param  Number activeTab New active tab
   */
  tabIndexChanged = (activeTab) => {
    this.refs.contentScrollView.scrollTo({
      x: SCREEN_WIDTH * activeTab,
      animated: !this.props.fixed
    });

    LayoutAnimation.configureNext(LayoutAnimation.Presets.easeInEaseOut);

    this.setState({
      activeTab
    }, () => {
      if (this.props.tabIndexChanged) {
        this.props.tabIndexChanged(activeTab);
      }
    });
  }

  /**
   * Wraps a tab button inside a TouchableOpacity, attaching relative handlers and props
   * @param  React.Component component Component to wrap
   * @param  String title Title of component
   * @param  Number index Position of tab button
   */
  renderWrappedTabButton(component, title, index) {
    return (
      <TouchableOpacity
        key={index}
        activeOpacity={0.6}
        hitSlop={HIT_SLOPE}
        onPress={() => this.tabIndexChanged(index, title)}
      >
        {component}
      </TouchableOpacity>
    );
  }

  /**
   * Renders a defaut tab button, with faded titles for an inactive tab
   * @param  String title Title of tab
   * @param  Number activeTab Currently active tab
   * @param  Number index Position of tab button
   */
  renderTabButton(title, activeTab, index) {
    if (this.props.renderTabButton) {
      return this.props.renderTabButton(title, activeTab, index);
    }

    return (
      <AnimatedEntrance
        style={styles.defaultTab}
        order={index + 1}
        translate='translateX'
      >
        <Text
          style={[
            styles.tabItemText, {
              color: activeTab === index ? this.props.theme.boldText : this.props.theme.tertiaryText
            }
          ]}
          allowFontScaling={false}
        >
          {titleCase(title)}
        </Text>
      </AnimatedEntrance>
    );
  }
  
  /**
   * @param  React.Component component JSX Component to wrap into view
   * @param  Number key Position of tab page (used for keying)
   */
  renderScrollTabContent(component, key) {
    return (
      <View
        key={key}
        style={styles.tabContent}
      >
        {component}
      </View>
    );
  }
  /**
   * Fires on ScrollView deceleration ending to calculate new active tab
   * @param  {} e
   */
  onMomentumScrollEnd = (e) => {
    const {
      contentOffset,
      layoutMeasurement
    } = e.nativeEvent;

    this.setState({
      activeTab: contentOffset.x / layoutMeasurement.width
    }, () => {
      if (this.props.tabIndexChanged) {
        this.props.tabIndexChanged(this.state.activeTab);
      }
    });
  }

  render() {
    return (
      <View>
        <ScrollView horizontal
          ref={'titleScrollView'}
          showsHorizontalScrollIndicator={false}
          style={styles.tabBarContainer}
          contentContainerStyle={styles.tabBarContent}
        >
          {this.props.children.map(({props}, index) => {
            const title = props ? props.title : 'unknown';

            return this.renderWrappedTabButton(
              this.renderTabButton(title, this.state.activeTab, index), title, index);
          })}
        </ScrollView>

        <ScrollView horizontal pagingEnabled
          ref={'contentScrollView'}
          scrollEnabled={!this.props.fixed}
          showsHorizontalScrollIndicator={false}
          onMomentumScrollEnd={this.onMomentumScrollEnd}
        >
          {this.props.children.map((c, i) => this.renderScrollTabContent(c, i))}
        </ScrollView>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  tabBarContainer: {
    flexDirection: 'row',
    paddingTop: 5
  },
  tabBarContent: {
    flexGrow: 1,
    justifyContent: 'center'
  },
  defaultTab: {
    paddingHorizontal: 15,
    paddingBottom: 10
  },
  tabItemText: {
    fontWeight: 'bold',
    fontSize: 16,
  },
  activeTabText: {
    color: '#000'
  },
  scrollContainer: {
    flex: 1
  },
  tabContent: {
    width: SCREEN_WIDTH
  }
});
