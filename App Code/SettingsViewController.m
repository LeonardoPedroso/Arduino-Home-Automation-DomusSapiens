//
//  SettingsViewController.m
//  DomusSapiens
//
//  Created by Leonardo Pedroso on 15/01/18.
//  Copyright © 2018 Leonardo Pedroso. All rights reserved.
//

#import "SettingsViewController.h"

@interface SettingsViewController ()


@end

@implementation SettingsViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    //make keyboard disappear
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(dismissKeyboard)];
    [self.view addGestureRecognizer:tap];

    NSUserDefaults *connection = [NSUserDefaults standardUserDefaults];
    
    _ipTextField.text = [connection objectForKey:IP];

    if ([[connection objectForKey:PORT] isEqualToString:@""]) {
        _portTextField.text = @"";
        [_portLabel setEnabled:FALSE];
        [_portTextField setEnabled:FALSE];
        [_portForwardingSwitchState setOn:FALSE];
    }
    else{
        _portTextField.text = [connection objectForKey:PORT];
        [_portLabel setEnabled:TRUE];
        [_portTextField setEnabled:TRUE];
        [_portForwardingSwitchState setOn:TRUE];
    }
    
    _passwordTextField.text = [connection objectForKey:PASSWORD];
    [_statusLabel setHidden:TRUE];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation
// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/
//test invalidity of fields
- (IBAction)connectButton:(id)sender {
    
    [_statusLabel setTextColor:[UIColor orangeColor]];
    _statusLabel.text = @"Connecting...";
    [_statusLabel setHidden:FALSE];
    
    if ([_ipTextField.text isEqualToString:@""]) {
        [self.statusLabel setTextColor:[UIColor redColor]];
        _statusLabel.text = @"Invalid IP address";
    }else if([_portForwardingSwitchState isOn] && [_portTextField.text isEqualToString:@""]){
        [self.statusLabel setTextColor:[UIColor redColor]];
        _statusLabel.text = @"Invalid port";
    }else if([_passwordTextField.text isEqualToString:@""]){
        [self.statusLabel setTextColor:[UIColor redColor]];
        _statusLabel.text = @"Invalid password";
    }
    else
    {
        NSUserDefaults *connection = [NSUserDefaults standardUserDefaults];
        [connection setObject:[NSString stringWithString:_ipTextField.text] forKey:IP];
        [connection setObject:[NSString stringWithString:_portTextField.text] forKey:PORT];
        [connection setObject:[NSString stringWithString:_passwordTextField.text] forKey:PASSWORD];
        [connection synchronize];
        
        [self performSelector:@selector(checkConnection) withObject:nil afterDelay:0.01];

        [[NSNotificationCenter defaultCenter] postNotificationName:@"update" object:nil];

    }
}

//change appearance accordingly to the connection status
-(void) checkConnection {
    if([deviceData testConnectionMain]>0){
        [_statusLabel setTextColor:[UIColor greenColor]];
        _statusLabel.text = @"Successfully connected";
    }else{
        [_statusLabel setTextColor:[UIColor redColor]];
        _statusLabel.text = @"Could not establish a connection";
    }
}

//test connection to main device
- (IBAction)testButton:(id)sender {
    [_statusLabel setTextColor:[UIColor orangeColor]];
    _statusLabel.text = @"Connecting...";
    [_statusLabel setHidden:FALSE];
    [self performSelector:@selector(testConnection) withObject:nil afterDelay:0.01];
}

//test connection to main devices and check how many secondary devices are connected
-(void) testConnection{
    int n = [deviceData testConnectionMainFull];
    [_statusLabel setHidden:FALSE];
    
    if (n>0 && n!= 1 && n!= 61) {
        [_statusLabel setTextColor:[UIColor greenColor]];
        _statusLabel.text = [NSString stringWithFormat:@"Succeesfully reached %i devices", n];
    }else if(n==1){
        [_statusLabel setTextColor:[UIColor greenColor]];
        _statusLabel.text = [NSString stringWithFormat:@"Succeesfully reached %i device", n];
    }else if(n == 61){
        [_statusLabel setTextColor:[UIColor orangeColor]];
        _statusLabel.text = [NSString stringWithFormat:@"No devices seem to be available"];
    }else{
        [_statusLabel setTextColor:[UIColor redColor]];
        _statusLabel.text = [NSString stringWithFormat:@"Connnection failed, unable to reach any device"];
    }

}

//switch beetween appearances
- (IBAction)portForwardingSwitchAction:(id)sender {
    if ([_portForwardingSwitchState isOn]) {
        [_portLabel setEnabled:TRUE];
        [_portTextField setEnabled:TRUE];
    }
    else{
        _portTextField.text = @"";
        [_portLabel setEnabled:FALSE];
        [_portTextField setEnabled:FALSE];
    }
}

-(void)dismissKeyboard
{
    [_ipTextField resignFirstResponder];
    [_portTextField resignFirstResponder];
    [_passwordTextField resignFirstResponder];
    [_statusLabel setHidden:TRUE];
}

@end
