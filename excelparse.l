snazzle.l
%{
  extern int yylex();
%}
%%

%%
void scan_string(const char* str)
{
    yy_switch_to_buffer(yy_scan_string(str));
}