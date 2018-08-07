//
//  MyTableViewController.h
//  DomusSapiens
//
//  Created by Leonardo Pedroso on 10/01/18.
//  Copyright © 2018 Leonardo Pedroso. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MyTableViewCell.h"
#import "deviceData.h"
#import "deviceViewController.h"
#import "MACROS_CONNNECTION.h"

@interface MyTableViewController : UITableViewController <returnDeviceDataDelegate>

-(void) disconnectedAppearance;
-(void) connectedAppearance;

@property (nonatomic) NSMutableDictionary * data;
@property (nonatomic, strong) NSString * connection;

- (IBAction)editButton:(id)sender;
@property (weak, nonatomic) IBOutlet UIBarButtonItem *editButtonOutlet;

@end
