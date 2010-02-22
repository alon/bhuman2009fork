using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace XabslEditor
{
  static class Program
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static void Main(string[] args)
    {
      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);
      if (args != null && args.Length >= 1 && args[0] != null && !args[0].Equals(""))
      {
        Application.Run(new MainForm(args[0]));
      }
      else
      {
        Application.Run(new MainForm());
      }
    }
  }
}