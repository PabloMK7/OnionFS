
#define LOG(msg, ...) { char buffer[0x200]; xsprintf(buffer, msg, ##__VA_ARGS__); WriteLog(buffer);}

typedef s32 Result;

void    InitLog(void);
void    ExitLog(void);
void    WriteLog(const char *log);
