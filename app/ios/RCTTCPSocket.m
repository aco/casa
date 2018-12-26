//
//  RCTTCPSocket.m
//  casa
//
//  Created by ac on 24/09/2018.
//  Copyright © 2018 Facebook. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "GCDAsyncSocket.h"

#import "RCTTCPSocket.h"

@implementation RCTTCPSocket

RCT_EXPORT_MODULE(TCPSocket);

#pragma mark - RCT method exports

RCT_EXPORT_METHOD(createConnection:(NSString *)host port:(int *)port disconnectOnStream:(BOOL *)disconnectOnStream)
{
//  uint16_t tPort = (unsigned short) port;

  self.dispatchSocket = [[GCDAsyncSocket alloc] initWithDelegate:self
                                                   delegateQueue:dispatch_get_main_queue()];
  
  self.dispatchSocket.delegate = self;
  self.dispatchSocket.autoDisconnectOnClosedReadStream = disconnectOnStream;
  
  [self.dispatchSocket connectToHost:host onPort:4045 withTimeout:-1 error:nil];
}

RCT_EXPORT_METHOD(writeString:(NSString *)data withTimeout:(double)timeout)
{
  NSLog(@"Writing: %@", data);
  
  [self.dispatchSocket writeData:[data dataUsingEncoding:NSASCIIStringEncoding]
                     withTimeout:timeout tag:0];
}

RCT_EXPORT_METHOD(destroy)
{
  [self.dispatchSocket disconnect];
}

- (NSArray<NSString *> *)supportedEvents
{
  return @[@"connect", @"data", @"error", @"close"];
}

#pragma mark - GCDAsyncSocketDelegate conformance

- (void)socket:(GCDAsyncSocket *)sock didConnectToHost:(NSString *)host port:(uint16_t)port
{
  [self sendEventWithName:@"connect" body:@{@"host": host}];
}

- (void)socket:(GCDAsyncSocket *)sock didReadData:(NSData *)data withTag:(long)tag
{
  NSLog(@"Received: %@", [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding]);
  
  [self sendEventWithName:@"data" body:@{@"message": [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding]}];
}

- (void)socket:(GCDAsyncSocket *)sock didWriteDataWithTag:(long)tag
{
  [self.dispatchSocket readDataWithTimeout:-1 tag:0]; // await receipt
}

- (void)socketDidDisconnect:(GCDAsyncSocket *)sock withError:(NSError *)err
{
  [self.dispatchSocket disconnect];
  [self sendEventWithName:@"error" body:@{@"errorMessage": err.localizedDescription}];
}

@end
