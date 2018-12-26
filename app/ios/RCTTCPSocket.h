//
//  RCTTCPSocket.h
//  casa
//
//  Created by ac on 24/09/2018.
//  Copyright © 2018 Facebook. All rights reserved.
//

#import <React/RCTView.h>

#import <React/RCTBridgeModule.h>
#import <React/RCTEventEmitter.h>

#import "GCDAsyncSocket.h"

@interface RCTTCPSocket : RCTEventEmitter<RCTBridgeModule, GCDAsyncSocketDelegate>

@property (strong, nonatomic) GCDAsyncSocket *dispatchSocket;

@end
