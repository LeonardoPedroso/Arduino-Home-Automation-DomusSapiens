//
//  SettingsViewController.h
//  DomusSapiens
//
//  Created by Leonardo Pedroso on 15/01/18.
//  Copyright © 2018 Leonardo Pedroso. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MACROS_CONNNECTION.h"
#import "deviceData.h"

@interface SettingsViewController : UIViewController
@property (weak, nonatomic) IBOutlet UINavigationItem *titleNavBar;

@property (strong, nonatomic) NSString * port;
@property (strong, nonatomic) NSString * ip;

@property (weak, nonatomic) IBOutlet UITextField *ipTextField;

@property (weak, nonatomic) IBOutlet UITextField *portTextField;
@property (weak, nonatomic) IBOutlet UISwitch *portForwardingSwitchState;
- (IBAction)portForwardingSwitchAction:(id)sender;

@property (weak, nonatomic) IBOutlet UITextField *passwordTextField;

@property (weak, nonatomic) IBOutlet UILabel *portLabel;

- (IBAction)connectButton:(id)sender;


- (IBAction)testButton:(id)sender;


@property (weak, nonatomic) IBOutlet UILabel *statusLabel;

@end
