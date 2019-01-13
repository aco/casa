import React from 'react';
import PropTypes from 'prop-types';

import {
  View,
  TouchableOpacity,
  StyleSheet,
  Slider,
  Switch,
  Image
} from 'react-native';

import {
  titleCase
} from '../util';

import Icons from '../asset/ico';

import {
  TabSlider
} from './';

import {
  CircularSlider,
  ColorPalette,
  SwitchSelect
} from '../control';

import {
  COMMAND_TYPE
} from '../constant/enum';

import AnimatedEntrance from 'react-native-animated-entrance';

import RGB_PALETTE from '../constant/rgbPalette';

/**
 * Generates and houses controls for a particular room
 */
export class RoomDashboard extends React.Component {

  static propTypes = {
    /**
     * Nodes within the room to render [{name, value, }]
     */
    nodes: PropTypes.array,
    /**
     * Instance of network/TCPSocket
     */
    dispatchSocket: PropTypes.object,
    /**
     * Handler to fire when a CircularSlider is engaged (used to disable parent ScrollView scrolling)
     */
    onTogglePan: PropTypes.func,
    /**
     * Object from constant/theme
     */
    theme: PropTypes.object
  }
  
  constructor(props) {
    super(props);

    this.state = {
      slider:  10,
      tabTypes: [],
      scrollEnabled: true
    };

    for (const node of props.nodes) { // pre-populate state with existing node values
      this.state[node.name] = node.value;
      this.state.tabTypes.push(node.type);
    }
  }

  /**
   * Locks ScrollView scrolling to prevent PanResponder clashes when panning
   */
  onTogglePan = () => {
    this.setState(prev => {
      return {
        scrollEnabled: !prev.scrollEnabled
      };
    });

    return this.props.onTogglePan();
  }

  /**
   * Renders a custom tab button for TabSlider using a related image asset
   * @param  String title Title of tab
   * @param  Number activeTab Currently active tab page
   * @param  Number index Position of tab element
   */
  renderTabButton = (title, activeTab, index) => {
    const currentTabActive = activeTab === index;

    return (
      <AnimatedEntrance
        translate='translateY'
        order={index + 1}

        style={[
          styles.tabButton,
          currentTabActive && {
            ...styles.tabButtonActive,
            backgroundColor: this.props.theme.backgroundColors[0],
            borderColor: this.props.theme.backgroundColors[0],
          }
        ]}
      >
        <Image
          resizeMode='contain'
          source={Icons.Icons[this.state.tabTypes[index]] || Icons.Icons.home}

          style={[
            styles.icon, {
            tintColor: currentTabActive ? this.props.theme.accentControl : this.props.theme.primaryText
          }]}
        />
      </AnimatedEntrance>
    );
  }

  /**
   * Renders a CircularSlider dial (e.g. brightness control)
   * @param  {} config Display config for slider
   * @param  String displaySuffix String appended to the end of the value
   * @param  Number upperBound Maximum slider value
   */
  renderSlider = (config, displaySuffix, upperBound) => {
    return (
        <CircularSlider
          width={240}
          height={240}

          upperBound={upperBound}
          rangedValue={this.state[config.name]}

          descriptionColor={this.props.theme.tertiaryText}
          meterColor={this.props.theme.accentControl}
          textColor={this.props.theme.primaryText}
          borderColor={this.props.theme.backgroundColors[1]}
          
          baseFontSize={18}
          description={titleCase(config.name)}
          displayValue={`${(this.state[config.name]).toFixed(0)}${displaySuffix}`}

          onTogglePan={this.onTogglePan}
          onValueChange={(value) => {
            this.setState({
              [config.name]: parseInt(value)
            });
          }}
          
          onPanRelease={this.props.dispatchSocket && (() => {
            this.props.dispatchSocket.write({
              type: COMMAND_TYPE.node,
              payload: {
                room: this.props.title,
                node: config.name,
                value: this.state[config.name]
              }
            });
          })}
        />
    );
  }

  /**
   * Renders a switch (i.e. SegmentControl)
   * @param  {} config Display config for slider
   * @param  {} options [{label, value}] Options to render
   * @param  Bool dispatches Whether toggling dispatches a socket message
   */
  renderSwitchSelect = (config, options, dispatches = true) => {
    return (
      <SwitchSelect hasPadding
        style={styles.switchSelect}
        initial={0}

        textColor={this.props.theme.accentControl}
        selectedColor='white'
        buttonColor={this.props.theme.accentControl}
        borderColor={this.props.theme.backgroundColors[1]}

        options={options} // [{label, value}]
        onPress={(value) => {
          this.setState({
            [config.name]: parseInt(value)
          }, (this.props.dispatchSocket) && (() => {
            this.props.dispatchSocket.write({
              type: COMMAND_TYPE.node,
              payload: {
                room: this.props.title,
                node: config.name,
                value: this.state[config.name]
              }
            });
          }));
        }}
      />
    );
  }

  /**
   * Renders a switch with a color palette below
   * @param  {} config
   * @param  {} options
   */
  renderColorControlSider = (config, options) => {
    return (
      <View style={styles.colorControl}>
        {this.renderSwitchSelect(config, [{
          label: 'Off',
          value: 0
        }, {
          label: 'On',
          value: 1
        }], false)}
        
        <ColorPalette
          style={styles.colorPalette}
          theme={this.props.theme}

          paletteRows={options}
          selectable={Boolean(this.state[config.name])}

          onSelectedColorChange={(color) => {
            this.props.dispatchSocket.write({
              type: COMMAND_TYPE.node,
              payload: {
                room: this.props.title,
                node: config.name,
                value: parseInt(color.substr(1), 16)
              }
            });
          }}
        />
      </View>
    );
  }

  /**
   * Renders an appropriate node JSX component for a config
   * @param  {} config {type, name}
   */
  renderNodeControl = (config) => {
    switch (config.type) {
      case 'alarm':
        return this.renderSwitchSelect(config, [{
          label: 'Disabled',
          value: 0
        }, {
          label: 'Active',
          value: 1
        }]);

        break;

      case 'garage':
      case 'door':
        return this.renderSwitchSelect(config, [{
          label: 'Closed',
          value: 0
        }, {
          label: 'Automatic',
          value: 50
        }, {
          label: 'Open',
          value: 100
        }]);

        break;

      case 'fireplace':
        return this.renderSwitchSelect(config, [{
          label: 'Off',
          value: 0
        }, {
          label: 'Static',
          value: 55
        }]);

        break;

      case 'light':
        return this.renderSlider(config, '%', 100);
        break;

      case 'rgb_light':
        return this.renderColorControlSider(config, RGB_PALETTE);
        break;

      case 'temperature':
        return this.renderSlider(config, '°C', 32);
        break;

      default:
        return (
          <View />
        );

        break;
    }
  }

  render() {
    return (
      <TabSlider bottomTabbed
        activeCircleColor={this.props.theme.accentControl}
        renderTabButton={this.renderTabButton}
        fixed={!this.state.scrollEnabled}
      >
        {this.props.nodes.map((node, index) => {
          return (
            <View
              key={index}
              title={node.name}
              style={styles.controlContainer}
            >
              {this.renderNodeControl(node)}
            </View>
          )
        })}
      </TabSlider>
    );
  }
}

const styles = StyleSheet.create({
  controlContainer: {
    flex: 1,
    paddingHorizontal: 10,
    paddingVertical: 5,
    alignItems: 'center'
  },
  tabButton: {
    width: 50,
    height: 50,
    alignItems: 'center',
    justifyContent: 'center',
    margin: 10,
  },
  tabButtonActive: {
    shadowOpacity: 0.2,
    shadowRadius: 5,
    shadowColor: 'black',
    shadowOffset: { height: 0, width: 0 },
    borderRadius:10,
    borderWidth: StyleSheet.hairlineWidth,
  },
  icon: {
    width: 25,
    height: 25
  },
  switchSelect: {
    flex: 1,
    justifyContent: 'center',
    paddingVertical: 80,
    paddingHorizontal: 10
  },
  colorControl: {
    alignItems: 'center'
  },
  colorPalette: {
    marginTop: 10
  }
});
