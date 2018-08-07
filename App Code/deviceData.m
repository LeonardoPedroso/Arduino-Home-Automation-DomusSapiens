//
//  deviceData.m
//  DomusSapiens
//
//  Created by Leonardo Pedroso on 11/01/18.
//  Copyright © 2018 Leonardo Pedroso. All rights reserved.
//

#import "deviceData.h"


@interface deviceData()
{
   
}

//private class method propotypes
+(NSString*) getJsonStringFromDict: (NSMutableDictionary *) dict;
+(NSString*) getUrl;
+(BOOL) sendCommandWithString:(NSString *)msg;
+(NSDictionary *) getJsonDictfromCommand: (NSString *)msg;

@end

@implementation deviceData

/*-------------------------------------------- testing --------------------------------------------------------*/

/*This method sends a request for main device asking to check the connection to each of the apired devices and return
 how many of them are available throuh JSON {"tma":#} which is returned*/
+(int) testConnectionMainFull;
{
    NSDictionary  * received = [deviceData getJsonDictfromCommand:MAIN_TEST];
    NSLog(@"CONNECTION MAIN DATAAAAA");
    if (received == nil) { //check if something was received
        return -1;
    }
    return (int)[received[HTTPOK] integerValue]; //return the number of avalable devices
}

/*-------------------------------------------- commands asking for object response ------------------------------*/
/*This method sends a resquest asking for the id of any deviced which is trying to be paired, which is returned
 Receives JSON in the form {"par":"dsxxxxxxx"}*/
+(NSString *) getParingDevice
{
    NSDictionary  * received = [deviceData getJsonDictfromCommand:PARING_DEVICE]; //get the data
    if (received == nil) { //check if something was received
        return nil;
    }
    NSLog(@"paring");
    NSLog(@"%@", received);
    return received[PARING_DEVICE]; //return
}

/*This method sends a resquest asking for the data of all the devices, which returns via a mutable dictionary*/
+(NSMutableDictionary *) getDevicesData
{
    #ifndef CONNECTION //debbuging purposes
    
    NSMutableDictionary * json = [[NSMutableDictionary alloc] init];
    NSData *data;
    NSString *url_string;
    
    int i = 1;
    while(TRUE)
    {
        url_string = [NSString stringWithFormat:@"%@%@", [deviceData getUrl],[NSString stringWithFormat:@"%@%02i", DATA, i]]; //setup url
        NSURLResponse* urlResponse;
        NSError* error;
        NSMutableURLRequest* urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url_string] cachePolicy:NSURLRequestReloadIgnoringCacheData timeoutInterval:10];
        data = [NSURLConnection sendSynchronousRequest:urlRequest returningResponse:&urlResponse error:&error];
    
        //data = [NSData dataWithContentsOfURL: [NSURL URLWithString:url_string]]; //get data
        
        if(error){
            NSLog(@"ERROR GETTING DATA");
            return nil;
        }
        
//        NSLog(@"GET DATAAA");
//        if (data == nil) {//check reception
//            return nil;
//        }
        id subJson = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];

        NSLog(@"%@", subJson);
       
        if ([subJson objectForKey:END_OF_RECEPTION])
            break;
        else{
            NSString * name = subJson[NAME];
            NSLog(@"%@", subJson);
            for (NSInteger charIdx=0; charIdx<name.length; charIdx++){
                if ([name characterAtIndex:charIdx] == '&')
                    name = [name stringByReplacingCharactersInRange:NSMakeRange(charIdx, 1) withString:[NSString stringWithFormat:@"%c", ' ']];
            }
            [subJson removeObjectForKey:NAME];
            [subJson setObject:name forKey:NAME];
            [json setObject:subJson forKey:[subJson objectForKey:ID]];
        }
        i++;
    }
    







//        NSDictionary  * received = [deviceData getJsonDictfromCommand:DATA]; //get the data
//        if (received == nil) {
//            return nil;
//        }
//        id json = [[deviceData getJsonDictfromCommand:DATA] mutableCopy]; //send a mutable copy of the data
    #else
        NSString * jsonString =@"{\"00000\":{\"H\":20,\"M\":2,\"h\":21,\"m\":2,\"F\":\"w\",\"f\":\"d\",\"n\":\"Bedroom\",\"i\":\"00000\",\"l\":\"Cima\"},\"00001\":{\"H\":10,\"M\":24,\"h\":4,\"m\":35,\"F\":\"w\",\"f\":\"e\",\"n\":\"Garage\",\"i\":\"00001\",\"l\":\"Baixo\"}}";
        NSData *data = [jsonString dataUsingEncoding:NSUTF8StringEncoding];
        id json = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
    #endif
    
    for (id key in json) {
        [json[key] setObject:@-1 forKey:ROW];
    }
    
    for (id key in json) {
        [json[key] setObject:DEFAULT_LABEL forKey:LABEL];
    }
    
    NSLog(@"%@", json);
    
        /*by default the rows are set to -1 in all the devices.
         This allows each user of the same Domus Sapiens system to have different configurations of the order in whitch the devices
         are presented. Therefore, a method that atributes row based on previous data is required.*/
    [deviceData atributeRows:json];
    [deviceData atributeLabels:json];
    return json;
}


/*-------------------------------------------- commands asking for object response ------------------------------*/

/*main method which sends command and waits for HTTPOK which is returned*/
+(BOOL) sendCommandWithString: (NSString *)msg
{
    #ifndef CONNECTION //debbuging purposes
        NSDictionary * received = [deviceData getJsonDictfromCommand:msg];
        if (received == nil) {
            return 0;
        }
        return [received[HTTPOK] integerValue];
    #endif
   
    NSLog(@"Sending %@", msg);
    NSLog(@"Successfully sent");
    return TRUE;
}

/*teste cnnection to main device*/
+(BOOL) testConnectionMain;
{
    return [deviceData sendCommandWithString:MAIN_TEST];
}

/*send commands to open or close a certain device*/
+(BOOL) sendCommandOpenCloseDirection:(NSString *)direction id:(NSString *)deviceId
{
    return [deviceData sendCommandWithString:[NSString stringWithFormat:@"%@%@", direction , deviceId]];
}

/*send JSON regarding edited device*/
+(BOOL) sendEditedDevice:(NSMutableDictionary *)deviceEditedData;
{  
    NSString * dataToSend = [deviceData getJsonStringFromDict:deviceEditedData];
    return [deviceData sendCommandWithString:[NSString stringWithFormat:@"%@%@", DEVICE_EDIT, dataToSend]];
}
/*send JSON regarding new device*/
+(BOOL) sendNewDevice:(NSMutableDictionary *)deviceNewData;
{
    NSString * dataToSend = [deviceData getJsonStringFromDict:deviceNewData];
    return [deviceData sendCommandWithString:[NSString stringWithFormat:@"%@%@",DEVICE_ADD, dataToSend]];
}
/*send id of device to delete*/
+(BOOL) sendIdToDelete:(NSString *)deviceId
{
    return [deviceData sendCommandWithString:[NSString stringWithFormat:@"%@%@",DEVICE_DELETE, deviceId]];
}

/*test connection with a particular device*/
+(BOOL) testConnectionDevice: (NSString *)deviceId
{
    return [deviceData sendCommandWithString:[NSString stringWithFormat:@"%@%@",DEVICE_TEST, deviceId]];
}


/*-------------------------------------------- auxiliary methods ------------------------------*/

/*method that returns url base on the connection entered by the user */
+(NSString *)getUrl
{
    NSUserDefaults *connection = [NSUserDefaults standardUserDefaults]; //opens user defaults
    NSString *url;
    NSString * ip = [connection objectForKey:IP];
    
    if ([[connection objectForKey:PORT] isEqualToString:@""]) { //check whether port forwarding is active
        url = [NSString stringWithFormat:@"http://%@/%@", ip,[connection objectForKey:PASSWORD]];
    }else{
        url = [NSString stringWithFormat:@"http://%@:%@/%@", ip, [connection objectForKey:PORT],[connection objectForKey:PASSWORD]];
    }
    return url;
}










/*method that returns string equivalent to dictionary in JSON notation*/
+(NSString*) getJsonStringFromDict:(NSMutableDictionary *) dict
{
    
    
    NSString * str = [NSString stringWithFormat:@"%@%@%02li%02li%@%02li%02li%@", dict[ID], dict[NAME], [dict[HOUR_ALARM_UP] integerValue], [dict[MINUTE_ALARM_UP] integerValue], dict[FREQ_ALARM_UP], [dict[HOUR_ALARM_DOWN] integerValue], [dict[MINUTE_ALARM_DOWN] integerValue], dict[FREQ_ALARM_DOWN]];
    NSLog(@"%@", str);
//    NSError *error;
//    //data parsing
//    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:dict options:(NSJSONWritingOptions) ( (0) ? NSJSONWritingPrettyPrinted : 0) error:&error];
//
//    if (! jsonData) { //check for an error
//        NSLog(@"bv_jsonStringWithPrettyPrint: error: %@", error.localizedDescription);
//        return @"{}";
//    } else {
//        return [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
//    }
    
    return str;
}


/*method which is responsible to atribute rows to all the devices paired, based on previous user configurations*/
+(void)atributeRows: (NSMutableDictionary *) data
{
    NSUserDefaults *defs = [NSUserDefaults standardUserDefaults];
    //dictionary may not exist
    if ([defs objectForKey:ROW_ORDER] == NULL) {
        [defs setObject:[[NSMutableDictionary alloc] init] forKey:ROW_ORDER];
    }
    //delete inexistent keys and atribute row to those already defined
    BOOL found = FALSE;
    int nFound = 0;
    for (id key in [defs objectForKey:ROW_ORDER]){
        found = FALSE;
        for (id keyData in data) {
            //NSLog(@"%@", [keyData s]);
            if ([key isEqualToString: keyData]) {
                data[keyData][ROW] = [defs objectForKey:ROW_ORDER][key];
                nFound++;
                found = TRUE;
                break;
            }
        }
        if(!found){
            //[[defs objectForKey:ROW_ORDER] removeObjectForKey:key];
        }
    }
    //order values for rows
    found = FALSE;
    for (int i = 0; i < nFound ; i++) {
        found = FALSE;
        for (id key in data) {
            if ([data[key][ROW] integerValue] == i) {
                found = TRUE;
                break;
            }
        }
        if(!found){ //if number i was not found
            for (id key in data) {
                if ([data[key][ROW] integerValue] > i) {
                    int aux =  (int)([data[key][ROW] integerValue] - 1);
                    [data[key] removeObjectForKey:ROW];
                    [data[key] setObject:[NSNumber numberWithInteger:aux] forKeyedSubscript:ROW];
                }
            }
            i--;
        }
    }
    
    //atribute row for the ones that stil are -1
    for (id key in data) {
        if ([data[key][ROW] integerValue] < 0) {
            [data[key] removeObjectForKey:ROW];
            [data[key] setObject:[NSNumber numberWithInteger:nFound] forKey:ROW];
            nFound++;
        }
    }
    
    //save configuration
    [deviceData saveRowOrder:data];
}



/*method in charge of saving current configuration of the rows of the user*/
+(void)saveRowOrder: (NSMutableDictionary *) data
{
    NSUserDefaults *defs = [NSUserDefaults standardUserDefaults]; //access user defautls
    [defs removeObjectForKey:ROW_ORDER]; //delete previous configuration
    
    NSMutableDictionary * rows =[[NSMutableDictionary alloc] init]; //setup and add configuration
    for (id key in data) {
        [rows setObject:data[key][ROW] forKey:key];
    }
    [defs setObject:rows forKey:ROW_ORDER];
    [defs synchronize]; //synch data
}













+(void)atributeLabels: (NSMutableDictionary *) data
{
    
    NSUserDefaults *defs = [NSUserDefaults standardUserDefaults];
    //dictionary may not exist
    if ([defs objectForKey:LABEL_DEFAULTS] == NULL) {
        [defs setObject:[[NSMutableDictionary alloc] init] forKey:LABEL_DEFAULTS];
    }
    //delete inexistent keys and atribute row to those already defined
    BOOL found = FALSE;
    int nFound = 0;
    for (id key in [defs objectForKey:LABEL_DEFAULTS]){
        found = FALSE;
        for (id keyData in data) {
            //NSLog(@"%@", [keyData s]);
            if ([key isEqualToString: keyData]) {
                data[keyData][LABEL] = [defs objectForKey:LABEL_DEFAULTS][key];
                nFound++;
                found = TRUE;
                break;
            }
        }
        if(!found){
            //[[defs objectForKey:LABEL_DEFAULTS] removeObjectForKey:key];
        }
    }
    

    //save configuration
    [deviceData saveLabelDefaults:data];
}

+(void)saveLabelDefaults: (NSMutableDictionary *) data
{
    NSUserDefaults *defs = [NSUserDefaults standardUserDefaults]; //access user defaults
    [defs removeObjectForKey:LABEL_DEFAULTS]; //delete previous configuration
    
    NSMutableDictionary * labels =[[NSMutableDictionary alloc] init]; //setup and add configuration
    for (id key in data) {
        [labels setObject:data[key][LABEL] forKey:key];
    }
    [defs setObject:labels forKey:LABEL_DEFAULTS];
    [defs synchronize]; //synch data
}

+(BOOL) checkInputData:(NSString *) str
{
    for (NSInteger charIdx=0; charIdx<str.length; charIdx++){
         char ch = [str characterAtIndex:charIdx];
        if ( !(ch == ' ' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) ) {
            return FALSE;
        }
    }
    return TRUE;
}

/*
+(NSString *) normalizeString:(NSString *) str
{
    BOOL nextIsCapital = TRUE;
    for (NSInteger charIdx=0; charIdx<str.length; charIdx++){
        char ch = [str characterAtIndex:charIdx];
        if(nextIsCapital && ch>= 97 && ch<= 122 && ch != ' '){
            
            ch = ch - 32;
            str = [str stringByReplacingCharactersInRange:NSMakeRange(charIdx, 1) withString:[NSString stringWithFormat:@"%c", ch]];
        }else if(!nextIsCapital && ch>= 'A' && ch <= 'Z' && ch != ' ')
        {
            ch = ch + 32;
            str = [str stringByReplacingCharactersInRange:NSMakeRange(charIdx, 1) withString:[NSString stringWithFormat:@"%c", ch]];
        }
        if (ch == ' ')
            nextIsCapital = TRUE;
        else
            nextIsCapital= FALSE;
    }
    return str;
}*/

/* method that sends a command to the main device and waits for a JSON response which is parsed into a dictionary to be returned*/
+(NSDictionary *) getJsonDictfromCommand:(NSString *) msg
{
    for (NSInteger charIdx=0; charIdx<msg.length; charIdx++){
        char ch = [msg characterAtIndex:charIdx];
        if (ch == ' ')
            msg = [msg stringByReplacingCharactersInRange:NSMakeRange(charIdx, 1) withString:[NSString stringWithFormat:@"%c", '&']];
       
    }
    NSMutableDictionary * json = [[NSMutableDictionary alloc] init];
    NSString *url_string = [NSString stringWithFormat:@"%@%@", [deviceData getUrl],msg]; //setup url
    NSURLResponse* urlResponse;
    NSError* error;
    NSMutableURLRequest* urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url_string] cachePolicy:NSURLRequestReloadIgnoringCacheData timeoutInterval:10];
    NSData* data = [NSURLConnection sendSynchronousRequest:urlRequest returningResponse:&urlResponse error:&error];
    
    NSLog(@"url %@", url_string);
    NSLog(@"data %@", data);
    if (data == nil) {//check reception
        return nil;
    }
    json = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
    NSLog(@"json %@", json);

    return json;
}

+(void) alertDevice{
    
}
@end
