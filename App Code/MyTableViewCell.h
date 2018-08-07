//
//  MyTableViewCell.h
//  DomusSapiens
//
//  Created by Leonardo Pedroso on 10/01/18.
//  Copyright © 2018 Leonardo Pedroso. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface MyTableViewCell : UITableViewCell

@property (strong, nonatomic) IBOutlet UILabel *nameLabel;
@property (strong, nonatomic) IBOutlet UILabel *timeUpLabel;
@property (strong, nonatomic) IBOutlet UILabel *timeDwLabel;
@property (strong, nonatomic) IBOutlet UILabel *freqUpLabel;
@property (strong, nonatomic) IBOutlet UILabel *freqDwLabel;
@property (strong, nonatomic) IBOutlet UILabel *UPLabel;
@property (strong, nonatomic) IBOutlet UILabel *DOWNLabel;

@end
