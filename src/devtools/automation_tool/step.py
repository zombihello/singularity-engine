# Automation step class
class Step:
    # Initialize the step
    def __init__( self, name, actionFunc, **kwargs ):
        self.name       = name
        self.actionFunc = actionFunc
        self.context    = kwargs

    # Execute the step
    def Execute( self ):
        self.actionFunc( self.context )