/**
 * Converts a string (typically JSON key) into title case.
 * @param  String str String to convert.
 * @example 'some_string' -> 'Some String'
 */
const titleCase = (str) => {
  const delimiter = str.includes('_') ? '_' : ' ';

  return str.toLowerCase().split(delimiter).map(w =>
    w.charAt(0).toUpperCase() + w.slice(1)).join(' ');
};

export {
  titleCase
};
