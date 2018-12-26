// import {
//   codec,
//   hash as SJHash,
// } from 'sjcl';

// export class Block {

//   constructor (index, prevBlock, data) {
//     this.index = index;
//     this.timestamp = Math.floor(new Date() / 1000);

//     this.hash = codec.utf8String.fromBits(
//       Hash.sha256.hash(index + prevBlock.hash + data));

//     this.prevBlock = prevBlock;

//     this.data = data;
//   }
// }

// export class Blockchain {

//   constructor() {
//     this._chain = new Block(0, undefined, 'Casa genesis block');
//   }

// }
