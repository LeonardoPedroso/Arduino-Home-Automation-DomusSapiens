//
//  MyTableViewController.m
//  DomusSapiens
//
//  Created by Leonardo Pedroso on 10/01/18.
//  Copyright © 2018 Leonardo Pedroso. All rights reserved.
//

#import "MyTableViewController.h"

@interface MyTableViewController ()

//define function
#define UIColorFromRGB(rgbValue) [UIColor colorWithRed:((float)((rgbValue & 0xFF0000) >> 16))/255.0 green:((float)((rgbValue & 0xFF00) >> 8))/255.0 blue:((float)(rgbValue & 0xFF))/255.0 alpha:0.7]

@end

@implementation MyTableViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    //ativate notification center to allow communication bettween view controllers
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(updateData) name:@"update" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(showConnectionFailMessage:) name:@"failD" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(cancelPairing:) name:@"cancelPairing" object:nil];
    self.data = [deviceData getDevicesData]; //get devices info
    if(!_data)
        [self disconnectedAppearance]; //show disconnected screen
    
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - notification center
- (void)dealloc
{
    //deactivate notification center
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"update" object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"failD" object:nil];
    //[super dealloc];
}

//function that updates the data in the tableview
- (void)updateData
{
    //get devices info
    self.data = [deviceData getDevicesData];
    if(_data)
        [self connectedAppearance]; //show connected screen
    else
        [self disconnectedAppearance]; //show disconnected screen
    [self.tableView reloadData]; //reaload table data
}

//function which is responsible for showing
- (void)showConnectionFailMessage:(NSNotification *) notification
{
    NSDictionary* userInfo = notification.userInfo; //put notification in a dictionary
    NSString * msg;
    //if the notification shows the pairing failed
    if([userInfo[@"M"] isEqualToString:@"PF"]){
        msg = @"Pairing failed, check the connection and try again.";
    }else{
        msg = [NSString stringWithFormat:@"Unable to reach device 'ds%@'\nCheck the connection and try again", userInfo[@"M"]];
    }
    [self alertMsg:msg]; //show alert msg
}

//function that is called to prevent errors when pairing is aborted
- (void)cancelPairing:(NSNotification *) notification
{
    NSLog(@"REMOVE RECENTLY PAIRED");
    NSDictionary* userInfo = notification.userInfo;
    //send command to the arduino to delete the device
    [deviceData sendIdToDelete:userInfo[@"M"]];
}

//function that shows a pop up message with msg
-(void)alertMsg:(NSString *)msg
{
    UIAlertController* alert = [UIAlertController alertControllerWithTitle:@"Connection failed"
                                                                   message:msg
                                                            preferredStyle:UIAlertControllerStyleAlert];
    
    UIAlertAction* defaultAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault
                                                          handler:^(UIAlertAction * action) {}];
    
    [alert addAction:defaultAction];
    [self presentViewController:alert animated:YES completion:nil];
    
}

//function that changes the screen to the disconnected appearance
-(void) disconnectedAppearance
{
    [self.navigationController.navigationBar setBarTintColor:[UIColor redColor]];
    [self.navigationItem.rightBarButtonItem setTintColor:[UIColor clearColor]];
    [self.navigationItem.leftBarButtonItem setTintColor:[UIColor clearColor]];
    [self.navigationItem.rightBarButtonItem setEnabled:NO];
    [self.navigationItem.leftBarButtonItem setEnabled:NO];
    self.navigationItem.title = @"Disconnected";
}




//function that changes the screen to the connected appearance
-(void) connectedAppearance
{
    [self.navigationController.navigationBar setBarTintColor:UIColorFromRGB(0x3C8994)];
    [self.navigationItem.rightBarButtonItem setTintColor:[UIColor whiteColor]];
    [self.navigationItem.leftBarButtonItem setTintColor:[UIColor whiteColor]];
    [self.navigationItem.rightBarButtonItem setEnabled:YES];
    [self.navigationItem.leftBarButtonItem setEnabled:YES];
    self.navigationItem.title = @"Devices";
}
#pragma mark - Table view data source
//functions for the number of sections and rows
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    if(!_data)
        return 0;
    return [[self.data allKeys] count];
}

//function which is in charge of organising the list of all the devices connected accordingly to the order selected by the user
//which is stored in the phone
- (MyTableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {

    MyTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"Cell" forIndexPath:indexPath];
    for(id key in self.data)
    {
        if ([[self.data objectForKeyedSubscript:key][ROW] integerValue] == indexPath.row) {
            //write device name to teh list
            cell.nameLabel.text = [self.data objectForKeyedSubscript:key][NAME];
            //write alarm up to the list if activated
            if ([[self.data objectForKeyedSubscript:key][FREQ_ALARM_UP] isEqualToString:DISABLED] || [self.tableView isEditing])
            {
                cell.timeUpLabel.hidden = TRUE;
                cell.UPLabel.hidden = TRUE;
                cell.freqUpLabel.hidden = TRUE;
            }
            else
            {
                cell.timeUpLabel.hidden = FALSE;
                cell.UPLabel.hidden = FALSE;
                cell.freqUpLabel.hidden = FALSE;
                cell.timeUpLabel.text = [NSString stringWithFormat:@"%02li:%02li",[[self.data objectForKeyedSubscript:key][HOUR_ALARM_UP] integerValue], [[self.data objectForKeyedSubscript:key][MINUTE_ALARM_UP] integerValue]];
                if([[self.data objectForKeyedSubscript:key][FREQ_ALARM_UP] isEqualToString:EVERYDAY])
                    cell.freqUpLabel.text = EVERYDAY_SHOW;
                else if([[self.data objectForKeyedSubscript:key][FREQ_ALARM_UP] isEqualToString:WEEKDAYS])
                    cell.freqUpLabel.text = WEEKDAYS_SHOW;
                else
                cell.freqUpLabel.text = WEEKENDS_SHOW;
            }
            //write alarm down to the list if activated
            if ([[self.data objectForKeyedSubscript:key][FREQ_ALARM_DOWN] isEqualToString:DISABLED] || [self.tableView isEditing])
            {
                cell.timeDwLabel.hidden = TRUE;
                cell.DOWNLabel.hidden = TRUE;
                cell.freqDwLabel.hidden = TRUE;
            }
            else
            {
                cell.timeDwLabel.hidden = FALSE;
                cell.DOWNLabel.hidden = FALSE;
                cell.freqDwLabel.hidden = FALSE;
                cell.timeDwLabel.text = [NSString stringWithFormat:@"%02li:%02li",[[self.data objectForKeyedSubscript:key][HOUR_ALARM_DOWN] integerValue], [[self.data objectForKeyedSubscript:key][MINUTE_ALARM_DOWN] integerValue]];
                if([[self.data objectForKeyedSubscript:key][FREQ_ALARM_DOWN] isEqualToString:EVERYDAY]){
                    cell.freqDwLabel.text = EVERYDAY_SHOW;
                }
                else if([[self.data objectForKeyedSubscript:key][FREQ_ALARM_DOWN] isEqualToString:WEEKDAYS]){
                     cell.freqDwLabel.text = WEEKDAYS_SHOW;
                }
                else{
                     cell.freqDwLabel.text = WEEKENDS_SHOW;
                }
            }
            break;
        }
    }
    return cell;
}

//called when deleting a device
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
   
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
        for (id key in _data) {
            if ([_data[key][ROW]  isEqualToNumber:[NSNumber numberWithInteger:indexPath.row]]) {
                if(![deviceData sendIdToDelete:_data[key][ID]])
                {
                    NSString * msg = [NSString stringWithFormat:@"Unable to reach device 'ds%@'\nCheck connection and try again", _data[key][ID]];
                    [self alertMsg:msg];
                    return;
                }
                [_data removeObjectForKey:key];
                break;
            }
        }
        for (id key in _data) {
            if ([_data[key][ROW] integerValue]  > indexPath.row) {
                NSNumber * aux = [NSNumber numberWithInteger:([_data[key][ROW] integerValue] - 1)];
                [_data[key] removeObjectForKey:ROW];
                [_data[key] setValue:aux forKey:ROW];
            }
        }
        
        [deviceData saveRowOrder:_data];
        [tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationFade];
        [tableView reloadData];
    }
}

//function to reajust list order and save it to the phone's storage
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath {
    
    id from = nil;
    id to = nil;
    for (id key in _data){
        if ([_data[key][ROW] isEqualToNumber:[NSNumber numberWithInteger:fromIndexPath.row]]) {
            from = key;
        }else if([_data[key][ROW] isEqualToNumber:[NSNumber numberWithInteger:toIndexPath.row]]){
            to = key;
        }
    }
    NSNumber * aux = _data[from][ROW];
    [_data[from] removeObjectForKey:ROW];
    [_data[from] setValue:_data[to][ROW] forKey:ROW];
    [_data[to] removeObjectForKey:ROW];
    [_data[to] setValue:aux forKey:ROW];
    [tableView reloadData];
    [deviceData saveRowOrder:_data];
}

- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath {
    return YES;
}

#pragma mark - Navigation

//preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    if ([sender isKindOfClass:[UITableViewCell class]] && [segue.destinationViewController isKindOfClass:[deviceViewController class]])
    {
        deviceViewController * nextDeviceView = segue.destinationViewController;
        NSIndexPath * path = [self.tableView indexPathForCell:sender];
        
        for (id key in self.data){
            if ([[self.data objectForKeyedSubscript:key][ROW] integerValue] == path.row){
                nextDeviceView.deviceData = [self.data objectForKey:key];
                break;
            }
        }
        nextDeviceView.delegate = self;
    }
    else if([sender isKindOfClass:[UIBarButtonItem class]] && [segue.destinationViewController isKindOfClass:[deviceViewController class]]){
        deviceViewController * nextDeviceView = segue.destinationViewController;
        nextDeviceView.deviceData = nil;
        nextDeviceView.delegate = self;
    }
    
}

#pragma mark - return data delegate
//adds new or edited device to the dictionary data
-(void)returnData:(NSMutableDictionary *)deviceDataDict
{
    BOOL exists = FALSE;
    for(id key in _data){
        if ([_data[key][ID] isEqualToString:deviceDataDict[ID]]) {
            [deviceDataDict removeObjectForKey:ROW];
            [deviceDataDict setObject:_data[key][ROW] forKey:ROW];
            [self.data removeObjectForKey:key];
            [self.data setObject:deviceDataDict forKey:key];
            exists = TRUE;
            if(![deviceData sendEditedDevice:_data[key]]){
                //erro
            }
            break;
        }
    }
    if(!exists){
        [deviceDataDict setValue:[NSNumber numberWithInteger:[[_data allKeys] count]] forKey:ROW];
        [self.data setObject:deviceDataDict forKey:deviceDataDict[ID]];
        /*if(![deviceData sendNewDevice:_data[deviceDataDict[ID]]]){
            //erro
        }*/
        if(![deviceData sendEditedDevice:_data[deviceDataDict[ID]]]){
            //erro
        }
    }
    [deviceData saveLabelDefaults:_data];
    [deviceData saveRowOrder:_data];
    [self.navigationController popViewControllerAnimated:YES];
    [self.tableView reloadData];
}

//change button from 'edit' to 'OK' and vice-versa
- (IBAction)editButton:(id)sender {
    
    if ([self.tableView isEditing]) {
        [self.tableView setEditing:NO animated:YES];
        [self.editButtonOutlet setTitle:@"Edit"];
    }
    else{
        [self.tableView setEditing:YES animated:YES];
        [self.editButtonOutlet setTitle:@"OK"];
    }
    [self.tableView reloadData];
    
}

@end
