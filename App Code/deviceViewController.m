//
//  deviceViewController.m
//  DomusSapiens
//
//  Created by Leonardo Pedroso on 12/01/18.
//  Copyright © 2018 Leonardo Pedroso. All rights reserved.
//

#import "deviceViewController.h"

@interface deviceViewController ()
{
    NSArray *_pickerData;
    NSMutableArray *_pickerData0;
    NSMutableArray *_pickerData1;
    NSArray *_pickerData2;
    
    BOOL dataToBeReturned;
    NSString * deviceID;
}
@end

@implementation deviceViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    //make keyboard disappear
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(dismissKeyboard)];
    [self.view addGestureRecognizer:tap];
    
    [_deviceNameTextFieldText resignFirstResponder];
    dataToBeReturned = FALSE;
    //appearance
    [self.navigationController.navigationBar setTintColor:[UIColor whiteColor]];
    
    
    if (_deviceData != nil) {
        _labelTextField.text = _deviceData[LABEL];
        _idLabel.text = [NSString stringWithFormat:@"ID: ds%@", _deviceData[ID]];
        
        if ([deviceData testConnectionDevice:_deviceData[ID]]) {
            _onlineLabel.text = @"Online";
            [_onlineLabel setTextColor:[UIColor greenColor]];
        }else{
            _onlineLabel.text = @"Offline";
            [_onlineLabel setTextColor:[UIColor redColor]];
            
            NSDictionary* userInfo = @{@"M": [_deviceData objectForKey:ID]};
            NSNotificationCenter* nc = [NSNotificationCenter defaultCenter];
            [self.navigationController popViewControllerAnimated:YES];
            [nc postNotificationName:@"failD" object:self userInfo:userInfo];
            
        }
    }
    else
    {
        //verificar se esta algum a emparelhar
        deviceID = [deviceData getParingDevice];
        _idLabel.text = [NSString stringWithFormat:@"ID: ds%@", deviceID];
        
        if ([deviceData testConnectionDevice:deviceID]) {
            _onlineLabel.text = @"Online";
            [_onlineLabel setTextColor:[UIColor greenColor]];
        }else{
            _onlineLabel.text = @"Offline";
            [_onlineLabel setTextColor:[UIColor redColor]];
            NSDictionary* userInfo = @{@"M": @"PF"};
            NSNotificationCenter* nc = [NSNotificationCenter defaultCenter];
            [self.navigationController popViewControllerAnimated:YES];
            [nc postNotificationName:@"failD" object:self userInfo:userInfo];
            
        }
        
        _labelTextField.text = @"default";
        
        [_openButtonOutlet setEnabled:FALSE];
        [_openButtonOutlet setAlpha:0.5];
        [_closeButtonOutlet setEnabled:FALSE];
        [_closeButtonOutlet setAlpha:0.5];
    }
    
    









    //pickerdata
    // Initialize Data
    _pickerData0 = [[NSMutableArray alloc] initWithCapacity:60];
    _pickerData1 = [[NSMutableArray alloc] initWithCapacity:24];
    _pickerData2 = @[@"ED", @"WD", @"WE"];
    for (int i = 0; i< 60; i++) {
        _pickerData1[i] = [NSString stringWithFormat:@"%02i", i];
    }
    for (int i = 0; i< 24; i++) {
        _pickerData0[i] = [NSString stringWithFormat:@"%02i", i];
    }
    _pickerData = [[NSArray alloc] initWithObjects:_pickerData0, _pickerData1, _pickerData2, nil];
    // Connect data
    self.alarmPickerUp.dataSource = self;
    self.alarmPickerUp.delegate = self;
    self.alarmPickerDw.dataSource = self;
    self.alarmPickerDw.delegate = self;
    
    /*-------------------------------------------inicial conditions---------------------------------*/

    //name
    [_invalidNameLabel setHidden:TRUE];
    if (_deviceData != nil) {
        _titleNavBar.title = _deviceData[NAME];
        _deviceNameTextFieldText.text = _deviceData[NAME];
    }else{
        _titleNavBar.title = @"New device";
        _deviceNameTextFieldText.text = nil;
    }
    
    //Alarm Up
    int k = 0;
    if (_deviceData != nil) {
        
        if ([_deviceData[FREQ_ALARM_UP] isEqualToString:WEEKDAYS]) {
            k=1;
        }else if([_deviceData[FREQ_ALARM_UP] isEqualToString:WEEKENDS]){
            k=2;
        }
        
        [self.alarmPickerUp selectRow:[_deviceData[HOUR_ALARM_UP] integerValue]  inComponent:0 animated:YES];
        [self.alarmPickerUp selectRow:[_deviceData[MINUTE_ALARM_UP] integerValue] inComponent:1 animated:YES];
        
    }else{
        NSDate *currentTime = [NSDate date];
        NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
        [dateFormatter setDateFormat:@"hh"];
        [self.alarmPickerUp selectRow:[[dateFormatter stringFromDate: currentTime] integerValue]  inComponent:0 animated:YES];
        [dateFormatter setDateFormat:@"mm"];
        [self.alarmPickerUp selectRow:[[dateFormatter stringFromDate: currentTime] integerValue] inComponent:1 animated:YES];
    }
    
    [self.alarmPickerUp selectRow:k inComponent:2 animated:YES];
    if ([_deviceData[FREQ_ALARM_UP] isEqualToString:DISABLED]) {
        [_disableAlarmUpState setOn:FALSE animated:TRUE];
        [_alarmPickerUp setUserInteractionEnabled:FALSE];
        [_alarmPickerUp setAlpha:0.6 ];
    }
    //Alarm Down
    k = 0;
    if(_deviceData != nil){
        if ([_deviceData[FREQ_ALARM_DOWN] isEqualToString:WEEKDAYS]) {
            k=1;
        }else if([_deviceData[FREQ_ALARM_DOWN] isEqualToString:WEEKENDS]){
            k=2;
        }
        [self.alarmPickerDw selectRow:[_deviceData[HOUR_ALARM_DOWN] integerValue]  inComponent:0 animated:YES];
        [self.alarmPickerDw selectRow:[_deviceData[MINUTE_ALARM_DOWN] integerValue] inComponent:1 animated:YES];
    }else{
        NSDate *currentTime = [NSDate date];
        NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
        [dateFormatter setDateFormat:@"hh"];
        [self.alarmPickerDw selectRow:[[dateFormatter stringFromDate: currentTime] integerValue]  inComponent:0 animated:YES];
        [dateFormatter setDateFormat:@"mm"];
        [self.alarmPickerDw selectRow:[[dateFormatter stringFromDate: currentTime] integerValue] inComponent:1 animated:YES];
        
    }
    
    [self.alarmPickerDw selectRow:k inComponent:2 animated:YES];
    if ([_deviceData[FREQ_ALARM_DOWN] isEqualToString:DISABLED]) {
        [_disableAlarmDwState setOn:FALSE animated:TRUE];
        [_alarmPickerDw setUserInteractionEnabled:FALSE];
        [_alarmPickerDw setAlpha:0.6 ];
    }
    
    
    
    [_invalidLabel setHidden:TRUE];
    
  
}



- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    
    // check if the back button was pressed
    if (self.isMovingFromParentViewController && !dataToBeReturned && !_deviceData) {
        
        NSDictionary* userInfo = @{@"M":deviceID};
        NSNotificationCenter* nc = [NSNotificationCenter defaultCenter];
        [self.navigationController popViewControllerAnimated:YES];
        [nc postNotificationName:@"cancelPairing" object:self userInfo:userInfo];
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}
/*-------------------------------------------------------------picker----------------------------------------_*/
// The number of columns of data
- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView
{
    return 3;
}

// The number of rows of data
- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component
{
    if([_pickerData[component] isEqualToArray:_pickerData0]){
        return 24;
    }
    else if([_pickerData[component] isEqualToArray:_pickerData1]){
        return 60;
    }
    else{
        return 3;
    }
}

// The data to return for the row and component (column) that's being passed in
- (NSString*)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component
{
    return _pickerData[component][row];
}



- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component
{
    // This method is triggered whenever the user makes a change to the picker selection.
    // The parameter named row and component represents what was selected.
    
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

/*-------------------------------------------------------------buttons----------------------------------------_*/
- (IBAction)openButton:(UIButton *)sender {
    [deviceData sendCommandOpenCloseDirection:OPEN id:_deviceData[ID]];
     [self.navigationController popViewControllerAnimated:YES];
}

- (IBAction)closeButton:(UIButton *)sender {
    [deviceData sendCommandOpenCloseDirection:CLOSE id:_deviceData[ID]];
     [self.navigationController popViewControllerAnimated:YES];
}

/*-------------------------------------------------------------text fields----------------------------------------_*/
- (IBAction)deviceNameTextField:(id)sender
{
    _titleNavBar.title = _deviceNameTextFieldText.text;
    if(![deviceData checkInputData:_deviceNameTextFieldText.text])
        [_invalidNameLabel  setHidden:FALSE];
    else
        [_invalidNameLabel  setHidden:TRUE];
}

-(void)dismissKeyboard
{
    [_deviceNameTextFieldText resignFirstResponder];
    [_labelTextField resignFirstResponder];
}





- (IBAction)disableAlarmUp:(id)sender
{
    if ([_disableAlarmUpState isOn]) {
        [_alarmPickerUp setUserInteractionEnabled:TRUE];
        [_alarmPickerUp setAlpha:1];
    }
    else{
        [_alarmPickerUp setUserInteractionEnabled:FALSE];
        [_alarmPickerUp setAlpha:0.6];
    }
    
}
- (IBAction)disableAlarmDw:(id)sender
{
    if ([_disableAlarmDwState isOn]) {
        [_alarmPickerDw setUserInteractionEnabled:TRUE];
        [_alarmPickerDw setAlpha:1];
    }
    else{
        [_alarmPickerDw setUserInteractionEnabled:FALSE];
        [_alarmPickerDw setAlpha:0.6];
    }
}

- (IBAction)doneButton:(UIBarButtonItem *)sender {
    NSMutableDictionary * dataToReturn = [self returnDeviceDataDict];
    if(dataToReturn!=nil){
        dataToBeReturned = TRUE;
        [self.delegate returnData:dataToReturn];
    }
}

-(NSMutableDictionary *) returnDeviceDataDict
{
    NSMutableDictionary *data = [[NSMutableDictionary alloc] init];
    
    BOOL valid = TRUE;
    if ([_deviceNameTextFieldText.text isEqualToString:@""] || ![deviceData checkInputData:_deviceNameTextFieldText.text]) {
        [_invalidNameLabel setHidden:FALSE];
        valid = FALSE;
    }
    [data setObject:_deviceNameTextFieldText.text forKey:NAME];
    [data setObject:_idLabel.text forKey:ID];
    [data setObject:[NSNumber numberWithInteger:[_alarmPickerUp selectedRowInComponent:0]] forKey:HOUR_ALARM_UP];
    [data setObject:[NSNumber numberWithInteger:[_alarmPickerUp selectedRowInComponent:1]] forKey:MINUTE_ALARM_UP];
    if([_pickerData2[[_alarmPickerUp selectedRowInComponent:2]] isEqualToString:EVERYDAY_SHOW])
        [data setObject:EVERYDAY forKey:FREQ_ALARM_UP];
    else if([_pickerData2[[_alarmPickerUp selectedRowInComponent:2]] isEqualToString:WEEKDAYS_SHOW])
        [data setObject:WEEKDAYS forKey:FREQ_ALARM_UP];
    else
        [data setObject:WEEKENDS forKey:FREQ_ALARM_UP];

    if (![_disableAlarmUpState isOn]) {
        [data setObject:DISABLED forKey:FREQ_ALARM_UP];
    }
    
    [data setObject:[NSNumber numberWithInteger:[_alarmPickerDw selectedRowInComponent:0]] forKey:HOUR_ALARM_DOWN];
    [data setObject:[NSNumber numberWithInteger:[_alarmPickerDw selectedRowInComponent:1]] forKey:MINUTE_ALARM_DOWN];
    
    if([_pickerData2[[_alarmPickerDw selectedRowInComponent:2]] isEqualToString:EVERYDAY_SHOW])
        [data setObject:EVERYDAY forKey:FREQ_ALARM_DOWN];
    else if([_pickerData2[[_alarmPickerDw selectedRowInComponent:2]] isEqualToString:WEEKDAYS_SHOW])
        [data setObject:WEEKDAYS forKey:FREQ_ALARM_DOWN];
    else
        [data setObject:WEEKENDS forKey:FREQ_ALARM_DOWN];
    
    if (![_disableAlarmDwState isOn]) {
        [data setObject:DISABLED forKey:FREQ_ALARM_DOWN];
    }
   
    [data setObject:[NSNumber numberWithInteger:-1] forKey:ROW];
    [data setObject:[_idLabel.text componentsSeparatedByString:@": ds"][1] forKey:ID]; //aqui erro para data nil
    if ([_labelTextField.text isEqualToString:@""] || ![deviceData checkInputData:_labelTextField.text]) {
        [_invalidLabel setHidden:FALSE];
        valid = FALSE;
    }
    [data setObject:_labelTextField.text forKey:LABEL];
    
    if (valid) {
        NSLog(@"%@", data);
        return data;
    }
    return nil;
}

- (IBAction)labelTextFieldAction:(id)sender {
    if(![deviceData checkInputData:_labelTextField.text])
        [_invalidLabel  setHidden:FALSE];
    else
        [_invalidLabel  setHidden:TRUE];
}
@end
