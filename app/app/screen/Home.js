import React from 'react';

import {
  StyleSheet,
  View,
  Dimensions,
  Modal,
  LayoutAnimation
} from 'react-native';

import {
  ParallaxHeader,
  RoomDashboard,
  TabSlider,
  HomeDashboard
} from '../component';

import {
  Configuration
} from './';

import {
  TCPSocket
} from '../provider';

import {
  COMMAND_TYPE
} from '../constant/enum';

import {
  DEMO_SYSTEM_STATS,
  DEMO_ROOM_STRUCTURE
} from '../constant/demoData';

import Images from '../asset/img';
import THEME from '../constant/theme';

import LinearGradient from 'react-native-linear-gradient';

import NotificationsIOS, {
  NotificationAction,
  NotificationCategory
} from 'react-native-notifications';

export default class Home extends React.Component {

  constructor(props) {
    super(props);

    this.state = {
      activeRoomIndex: 0,
      scrollEnabled: true,
      theme: 'light',
      rooms: [],
      stats: []
    };
  }

  /**
   * Instantiates a TCP socket using a given config.
   * @param  {} config {host, port}
   */
  onSubmitConfiguration = (config) => {
    LayoutAnimation.configureNext(LayoutAnimation.Presets.easeInEaseOut);

    if (!config) { // run as demo
      this.setState({
        rooms: DEMO_ROOM_STRUCTURE,
        stats: DEMO_SYSTEM_STATS
      });

      return;
    }

    this.socket = new TCPSocket(config.host, config.port);
    
    this.socket.onData = (data) => {
      try {
        let json = JSON.parse(data.message);

        switch (json.type) {
          case COMMAND_TYPE.node:
            
            break;
          case COMMAND_TYPE.report:

            switch (json.payload.type) {
              case 'rooms':
                this.setState({
                  rooms: json.payload.value
                });
                
                break;

              case 'system':
                this.setState({
                  stats: json.payload.value
                });

                break;
              default:
                break;
            }
            
            break;

          case COMMAND_TYPE.notification:
            switch (json.payload.type) {
              case 'daylight':
                this.setState({
                  theme: json.payload.value === true ? 'dark' : 'light'
                });


                break;
            
              default:
                break;
            }
            
            break;
          default:
            break;
        }
      } catch (error) {
        console.log('Unexpected data', data, error);
      }

      this.socket.write({
        type: COMMAND_TYPE.confirmation
      });
    };

    this.socket.write({ // send device identification
      type: COMMAND_TYPE.identification,
      payload: {
        value: config.profile
      }
    });
  }

  /**
   * Called when changing tab page, stores the active tab index
   * @param  Number activeTab
   */
  tabIndexChanged = (activeTab) => {
    this.setState({
      activeRoomIndex: activeTab
    });
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

    return true;
  }

  render() {
    const homeTabActive = this.state.activeRoomIndex === 0;
    const tabSliderBody = this.state.rooms.length === 0 ? [(
      <Configuration
        title='Configure'
        key={0}
        onSubmitConfiguration={this.onSubmitConfiguration}
      />
    )] : this.state.rooms.map((room, index) => (
      <RoomDashboard
        key={index + 2}
        title={room.name}
        nodes={room.nodes}
        theme={THEME[this.state.theme]}
        onTogglePan={this.onTogglePan}
        dispatchSocket={this.socket}
      />
    ));

    if (this.state.stats.length) {
      tabSliderBody.unshift((
        <HomeDashboard
          key={1}
          title='Dashboard'
          theme={THEME[this.state.theme]}
          stats={this.state.stats}
        />
      ));
    }

    return (
      <ParallaxHeader
        title={homeTabActive ? 'casa' : `${this.state.rooms[this.state.activeRoomIndex - 1].temperature}°C`}
        titleStyle={homeTabActive && styles.casa}
        titleMaxFontSize={homeTabActive ? 56 : 30}
        minFontReduction={homeTabActive ? 30 : 12}

        scrollEnabled={this.state.scrollEnabled}
        backgroundImage={Images.Headers[homeTabActive ? this.state.theme : this.state.rooms[this.state.activeRoomIndex - 1].name] || 0}
        backgroundGradientColors={THEME[this.state.theme].backgroundColors}
      > 
        <TabSlider fixed
          tabIndexChanged={this.tabIndexChanged}
          theme={THEME[this.state.theme]}
        >
          {tabSliderBody}
        </TabSlider>
      </ParallaxHeader>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    paddingBottom: 150
  },
  header: {
    margin: 10,
    fontSize: 26,
  },
  casa: {
    fontFamily: 'Damion'
  }
});
