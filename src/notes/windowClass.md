

# styles 


The class styles define additional elements of the window class. 
Two or more styles can be combined by using the bitwise OR (|) operator. 
To assign a style to a window class, 
assign the style to the style member of the WNDCLASSEX structure. 
For a list of class styles, see Window Class Styles.


## styles we need to be aware off

device context - something to draw on that windows OS can give, like 2d context etc, can have state. 


### CS_CLASSDC 0x0040
Allocates one device context to be shared by all windows in the class.
Because window classes are process specific, 
it is possible for multiple threads of an application to create a window of the same class. 
It is also possible for the threads to attempt to use the device context simultaneously. 
When this happens,
the system allows only one thread to successfully finish its drawing operation.

### CS_OWNDC 0x0020

Allocates a unique device context for each window in the class.
  here we won't need to get and release the context, we own it.

# lpfnWndProc


a pointer to the window procedure, must call  CallWindowproc, to call the window procedure

handle messages from windows OS, that pertain to us
