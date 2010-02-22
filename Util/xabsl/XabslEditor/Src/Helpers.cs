using System;
using System.Net;
using System.Reflection;
using System.Windows.Forms;
using System.Collections.Generic;

namespace XabslEditor
{
  public class Helpers
  {
    #region SCINTILLA constants

    public const int SCI_INDICS_MASK = 0xE0;
    public const int SCI_INDIC0_MASK = 0x20;
    public const int SCI_INDIC1_MASK = 0x40;
    public const int SCI_INDIC2_MASK = 0x20;
    
    #endregion
   
    public static string getSystemInfo()
    {
      Assembly asm = Assembly.GetExecutingAssembly();
      AssemblyDescriptionAttribute[] desc =
        (AssemblyDescriptionAttribute[])
        asm.GetCustomAttributes(typeof(System.Reflection.AssemblyDescriptionAttribute), false);

      string svnRev = desc[0].Description;

      return
        "SVN: " + svnRev + "\r\n" +
        "System Time: " + DateTime.Now.ToString() + "\r\n" +
        "User: " + SystemInformation.UserName + "\r\n" +
        "Computer: " + SystemInformation.ComputerName + "\r\n" +
        "CurrentDirectory: " + System.Environment.CurrentDirectory + "\r\n" +
        "OS Version: " + System.Environment.OSVersion + "\r\n" +
        ".Net Version: " + System.Environment.Version + "\r\n" +
        "Working Set: " + System.Environment.WorkingSet / 1024 / 1024 + "MB \r\n";
    }
  } 

}