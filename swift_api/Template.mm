#import <Foundation/Foundation.h>
#import "Template.h"
#include "pbio/Template.h"
#include <fstream>
#include <iostream>

#import "TryCatch.mm"

@implementation Template

-(id)init:(void *)temp{
    _template = temp;
    return self;
}

-(void) dealloc{
    delete static_cast<pbio::Template::Ptr*>(_template);
}

-(NSString*) getMethodName{
    pbio::Template::Ptr temp = (*(pbio::Template::Ptr*)_template);
    
    return tryCatch(@(temp->getMethodName().c_str()));
}

-(void) save:(const char *)file_path{
    pbio::Template::Ptr temp = (*(pbio::Template::Ptr*)_template);
    
    std::ofstream stream(file_path, std::ios_base::binary);
    
    if (stream.is_open()){
        tryCatch(temp->save(stream));
        stream.close();
    }
}
@synthesize _template;

@end
