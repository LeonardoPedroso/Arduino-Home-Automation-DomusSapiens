//
//  deviceViewController.h
//  DomusSapiens
//
//  Created by Leonardo Pedroso on 12/01/18.
//  Copyright © 2018 Leonardo Pedroso. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MACROS_DATA.h"
#import "deviceData.h"
#import "MACROS_CONNNECTION.h"

@protocol returnDeviceDataDelegate <NSObject>

@required
-(void)returnData: (NSMutableDictionary *) deviceDataDict;

@end


@interface deviceViewController : UIViewController <UIPickerViewDataSource, UIPickerViewDelegate>

//delegate
@property (weak, nonatomic) id <returnDeviceDataDelegate> delegate;

//data
@property (nonatomic, strong) NSMutableDictionary * deviceData;

//OK button
- (IBAction)doneButton:(UIBarButtonItem *)sender;

//text field
@property (weak, nonatomic) IBOutlet UITextField *deviceNameTextFieldText;
@property (weak, nonatomic) IBOutlet UILabel *invalidNameLabel;
- (IBAction)deviceNameTextField:(id)sender;
@property (weak, nonatomic) IBOutlet UINavigationItem *titleNavBar;


//buttons open close
- (IBAction)openButton:(UIButton *)sender;
- (IBAction)closeButton:(UIButton *)sender;
@property (weak, nonatomic) IBOutlet UIButton *openButtonOutlet;
@property (weak, nonatomic) IBOutlet UIButton *closeButtonOutlet;

//alarm up
@property (weak, nonatomic) IBOutlet UISwitch *disableAlarmUpState;
- (IBAction)disableAlarmUp:(id)sender;
@property (weak, nonatomic) IBOutlet UIPickerView *alarmPickerUp;

//alarm down
@property (weak, nonatomic) IBOutlet UISwitch *disableAlarmDwState;
- (IBAction)disableAlarmDw:(id)sender;
@property (weak, nonatomic) IBOutlet UIPickerView *alarmPickerDw;

//label
@property (weak, nonatomic) IBOutlet UITextField *labelTextField;
@property (weak, nonatomic) IBOutlet UILabel *invalidLabel;
- (IBAction)labelTextFieldAction:(id)sender;

//id
@property (weak, nonatomic) IBOutlet UILabel *idLabel;
@property (weak, nonatomic) IBOutlet UILabel *onlineLabel;


@end


