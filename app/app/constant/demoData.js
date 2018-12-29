const DEMO_SYSTEM_STATS = [{
  name: 'uptime',
  value: 7200,
  ubound: 60
}, {
  name: 'usage',
  value: 13,
  ubound: 100,
  suffix: 2
}, {
  name: 'sys_temp',
  value: 49,
  ubound: 85,
  suffix: 1
}];

const DEMO_ROOM_STRUCTURE = [{
  name: 'home',
  temperature: 24,
  nodes: [{
    name: 'main_alarm',
    type: 'alarm',
    value: true
  }, {
    name: 'side_lighting',
    type: 'rgb_light',
    value: 60
  }, {
    name: 'porch_light',
    type: 'light',
    value: 10
  }, {
    name: 'central_heating',
    type: 'temperature',
    value: 23
  }]
}, {
  name: 'lounge',
  temperature: 23,
  nodes: [{
    name: 'ceiling_light',
    type: 'light',
    value: 50
  }, {
    name: 'fireplace',
    type: 'fireplace',
    value: true
  }]
}, {
  name: 'kitchen',
  temperature: 23.5,
  nodes: [{
    name: 'ceiling_light',
    type: 'light',
    value: 0
  }]
}, {
  name: 'bedroom',
  temperature: 22,
  nodes: [{
    name: 'ceiling_light',
    type: 'light',
    value: 95
  }]
}, {
  name: 'garage',
  temperature: 18.3,
  nodes: [{
    name: 'entrance_light',
    type: 'light',
    value: 40
  }, {
    name: 'entrance',
    type: 'garage',
    value: 0
  }]
}];

export {
  DEMO_ROOM_STRUCTURE,
  DEMO_SYSTEM_STATS
};
