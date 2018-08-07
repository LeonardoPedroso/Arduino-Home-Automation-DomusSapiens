//
//  deviceData.h
//  DomusSapiens
//
//  Created by Leonardo Pedroso on 11/01/18.
//  Copyright © 2018 Leonardo Pedroso. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MACROS_DATA.h"
#import "MACROS_CONNNECTION.h"

@interface deviceData : NSObject

#pragma mark - testing
+(BOOL) testConnectionMain;
+(int) testConnectionMainFull;
+(BOOL) testConnectionDevice: (NSString *)deviceId;

#pragma mark - commands with object response
+(NSString *) getParingDevice;
+(NSMutableDictionary *) getDevicesData;

#pragma mark - commands with HTTP OK response
+(BOOL) sendCommandOpenCloseDirection:(NSString *)direction id:(NSString *)deviceId;

+(BOOL) sendEditedDevice:(NSMutableDictionary *)deviceEditedData;
+(BOOL) sendNewDevice:(NSMutableDictionary *)deviceNewData;
+(BOOL) sendIdToDelete:(NSString *)deviceId;

#pragma mark - manage row order
+(void)atributeRows: (NSMutableDictionary *) data;
+(void)saveRowOrder: (NSMutableDictionary *) data;
+(void)atributeLabels: (NSMutableDictionary *) data;
+(void)saveLabelDefaults: (NSMutableDictionary *) data;

+(BOOL) checkInputData:(NSString *) str;

+(void) alertDevice;


@end
