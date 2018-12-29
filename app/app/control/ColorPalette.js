import React from 'react';
import PropTypes from 'prop-types';

import { 
  View,
  Text,
  StyleSheet,
  TouchableOpacity
} from 'react-native';

const OUTER_CIRCLE_DIAMETER = 28;
const INNER_CIRCLE_DIAMETER = 20;

const hit_slop = {
  top: 10,
  left: 20,
  right: 20,
  bottom: 10
}

export class ColorPalette extends React.Component {

  static defaultProps = {
    /**
     * Fired with the new value color on change
     */
    onSelectedColorChange: PropTypes.func,
    /**
     * Theme object from constant/Theme
     */
    theme: PropTypes.object,
    /**
     * [[{displayColor, valueColor}]]
     */
    paletteRows: PropTypes.array,
    /**
     * Whether colors are selectable
     */
    selectable: PropTypes.bool
  }

  constructor(props) {
    super(props);

    const rowLength = props.paletteRows.length - 1;
    const colLength = props.paletteRows[rowLength].length - 1

    this.state = {
      selectedColor: props.paletteRows[rowLength][colLength].displayColor
    };
  }
  /**
   * Fired when selected color is changed, invokes prop passing selected's valueColor.
   * @param  {} color Selected Color object.
   */
  onSelectedColorChange = (color) => {
    this.setState({
      selectedColor: color.displayColor
    }, () => {
      if (this.props.onSelectedColorChange) {
        this.props.onSelectedColorChange(color.valueColor);
      }
    });
  }
  /**
   * Renders a color spot.
   * @param  {} color Color object to render.
   * @param  Number key Position of spot.
   */
  renderColorSpot = (color, key) => {
    const activeColor = this.props.selectable && this.state.selectedColor === color.displayColor;

    return (
        <TouchableOpacity
          key={key}

          activeOpacity={0.6}
          hitSlop={hit_slop}

          onPress={() => {
            if (this.props.selectable) {
              this.onSelectedColorChange(color);
            }
          }}
          
          style={[styles.spotContainer, {
            borderColor: this.props.theme.backgroundColors[0]
          }, activeColor && {
            borderColor: this.props.theme.accentControl,
            backgroundColor: this.props.theme.accentControl,

            shadowOpacity: 0.2,
            shadowRadius: 5,
            shadowColor: 'black',
            shadowOffset: { height: 0, width: 0 },
          }]}
        >
          <View style={[styles.spot, {
            backgroundColor: color.displayColor
          }]} />
        </TouchableOpacity>
    )
  }
  /**
   * Renders a row of color spots
   * @param  Array [{displayColor, valueColor}]
   * @param  Number key Position of row.
   */
  renderPaletteRow = (colors, key) => {
    return (
      <View
        key={key}
        style={styles.paletteRow}
      >
        {colors.map((c, i) => this.renderColorSpot(c, i))}
      </View>
    );
  }

  render() {
    return (
      <View style={styles.container}>
        {this.props.paletteRows.map((c, i) => this.renderPaletteRow(c, i))}
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1
  },
  paletteRow: {
    flex: 1,
    flexDirection: 'row',
    justifyContent: 'center'
  },
  spotContainer: {
    width: OUTER_CIRCLE_DIAMETER,
    height: OUTER_CIRCLE_DIAMETER,
    borderRadius: OUTER_CIRCLE_DIAMETER / 2,

    marginHorizontal: 20,
    marginVertical: 6,
    alignItems: 'center',
    justifyContent: 'center',
    borderWidth: StyleSheet.hairlineWidth,
  },
  spot: {
    width: INNER_CIRCLE_DIAMETER,
    height: INNER_CIRCLE_DIAMETER,
    borderRadius: INNER_CIRCLE_DIAMETER / 2
  }
});
