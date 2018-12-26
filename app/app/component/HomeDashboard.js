import React from 'react';
import PropTypes from 'prop-types';

import { 
  View,
  Text,
  StyleSheet,
  InteractionManager
} from 'react-native';

import {
  CircularSlider
} from '../control';

import {
  titleCase
} from '../util';

import {
  STAT_SUFFIX
} from '../constant/enum';

/**
 * Renders numerous stats in a 2x* grid of dials
 */
export class HomeDashboard extends React.Component {

  static propTypes = {
    /**
     * Array of stats to render as dials in format [{name, value, suffix, ubound}]
     * Use 'uptime' as the name for a stat for live incrementation
     */
    stats: PropTypes.array,
    /**
     * Theme from constant/theme
     */
    theme: PropTypes.object
  }

  constructor(props) {
    super(props);

    const uptime = props.stats.find(s => s.name === 'uptime').value;

    this.state = {
      uptimeRange: 1,
      uptimeDisplay: this.formatUptimeDisplay(uptime),
      uptime,
    };

    this.timer = this.updateUptime();
  }

  componentWillUnmount() {
    clearTimeout(this.timer);
  }

  /**
   * Formats a unix timestamp into relative time
   * @param  Number uptime
   * @example 15392304823984920 -> '2 hr'
   */
  formatUptimeDisplay = (uptime) => {
    let hours = Math.floor(uptime / 3600);
    let minutes = Math.floor((uptime - (hours * 3600)) / 60);
    let seconds = uptime - (hours * 3600) - (minutes * 60);

    if (hours > 0) {
      return hours + ' hr';
    } else if (minutes > 0) {
      return minutes + ' min';
    } else {
      return seconds + ' secs';
    }
  }

  /**
   * Repeated event incrementing and formatting uptime
   */
  updateUptime = () => {
    InteractionManager.runAfterInteractions(() => { // Queues render until after interactions
      this.setState(prevState => {
        return {
          uptimeRange: prevState.uptimeRange > 59 ? 1 : prevState.uptimeRange + 1,
          uptime: prevState.uptime + 1,
          uptimeDisplay: this.formatUptimeDisplay(this.state.uptime + 1)
        }
      });

      this.timer = setTimeout(() => {
        this.updateUptime();
      }, 1000);
    });
  }

  /**
   * Renders a JSX dial
   * @param  {} stat Stat to render {name, value, suffix, ubound}
   * @param  {} index Position of stat
   */
  renderCircularDial = (stat, index) => {
    return (
      <CircularSlider display
        key={index}
        style={styles.circularSlider}
        width={160}
        height={160}

        upperBound={stat.ubound + 1}
        rangedValue={this.state[`${stat.name}Range`] || stat.value}

        descriptionColor={this.props.theme.tertiaryText}
        meterColor={this.props.theme.accentControl}
        textColor={this.props.theme.primaryText}
        borderColor={this.props.theme.backgroundColors[1]}
        
        baseFontSize={14}
        description={titleCase(stat.name)}
        displayValue={this.state[`${stat.name}Display`] || `${stat.value.toFixed(0)}${STAT_SUFFIX[stat.suffix]}`}
      />
    );
  }

  render() {
    return (
      <View
        style={styles.container}
      >
        {this.props.stats.map(this.renderCircularDial)}
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    width: '100%', 
    flexDirection: 'row', 
    flexWrap: 'wrap',
    justifyContent: 'space-between',
    padding: 20
  },
  circularSlider: {
    marginBottom: 20
  }
});
