def CHRESET:I<320,(outs), (ins),[], "CHRESET">; //Reset channel
def CHSTART:I<321,(outs), (ins),[], "CHSTART">; //Start sound on channel
def CHSTOP:I<322,(outs), (ins),[], "CHSTOP">; //Stop sound on channel
def WSET:I<330,(outs), (ins),[], "WSET">; //Set lookup name for specific wave
def CHWAVE:I<331,(outs), (ins),[], "CHWAVE">; //Set waveform
def CHLOOP:I<332,(outs), (ins),[], "CHLOOP">; //Set looping mode
def CHVOLUME:I<333,(outs), (ins),[], "CHVOLUME">; //Set volume
def CHPITCH:I<334,(outs), (ins),[], "CHPITCH">; //Set pitch (value interpretation depends on register)
def CHMODT:I<335,(outs), (ins),[], "CHMODT">; //Set LFO modulation type
def CHMODA:I<336,(outs), (ins),[], "CHMODA">; //Set LFO modulation amplitude
def CHMODF:I<337,(outs), (ins),[], "CHMODF">; //Set LFO modulation frequency
def CHADSR:I<338,(outs), (ins),[], "CHADSR">; //Set channel ADSR
def WLEN:I<339,(outs), (ins),[], "WLEN">; //Read sound length in seconds