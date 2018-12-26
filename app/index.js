import {
  StatusBar,
  AppRegistry
} from 'react-native';

import Home from './app/screen/Home';

import {
  name as appName
} from './app.json';

StatusBar.setBarStyle('light-content', true);
AppRegistry.registerComponent(appName, () => Home);