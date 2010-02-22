using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace XabslEditor
{

  /// <summary>
  /// Represents a tab including the document (XabslDocument) and a view (ScintillaControl).
  /// Simple data-structure with public members. Manipulation has to be done by the UI.
  /// </summary>
  public class Tab
  {
    public XabslDocument doc;
    public Scintilla.ScintillaControl view;

    public List<string> backOptions;
    public List<string> forwardOptions;

    public bool wasSaved;

    private Control _parent;

    public Tab(XabslDocument doc, Control parent, string xabslStyleFile)
    {
      this.doc = doc;
      this.view = new Scintilla.ScintillaControl();
      this._parent = parent;

      backOptions = new List<string>();
      forwardOptions = new List<string>();

      wasSaved = true;

      try
      {
        Scintilla.Configuration.ConfigurationUtility cu = new Scintilla.Configuration.ConfigurationUtility(GetType().Module.Assembly);
        Scintilla.Configuration.Scintilla conf = (Scintilla.Configuration.Scintilla)cu.LoadConfiguration(
          xabslStyleFile);

        view.Configuration = conf;
        view.ConfigurationLanguage = "xabsl";

        // show line-numbers
        view.SetMarginWidthN(0, 30);
        // no codefolding yet
        view.SetMarginWidthN(1, 0);
        // tabs with spaces and width of 2
        view.TabWidth = 2;
        view.IsUseTabs = false;
        // indent
        view.Indent = 2;
        view.IsIndentationGuides = true;

        // don't use these keys 
        view.AddIgnoredKey(Shortcut.CtrlO);
        view.AddIgnoredKey(Shortcut.CtrlS);
        view.AddIgnoredKey(Shortcut.F5);
        view.AddIgnoredKey(Keys.Space, Keys.Control);
        view.AddIgnoredKey(Shortcut.CtrlZ);
        view.AddIgnoredKey(Shortcut.CtrlY);
        view.AddIgnoredKey(Shortcut.CtrlB);
        view.AddIgnoredKey(Shortcut.CtrlF);
        view.AddIgnoredKey(Shortcut.CtrlShiftF);
        view.AddIgnoredKey(Shortcut.CtrlN);
        view.AddIgnoredKey(Shortcut.CtrlI);
        view.AddIgnoredKey(Shortcut.CtrlT);
        view.AddIgnoredKey(Shortcut.CtrlW);
        view.AddIgnoredKey(Shortcut.Ctrl7);
        view.AddIgnoredKey(Shortcut.Ctrl8);

        view.AddIgnoredKey(Shortcut.F6);
        view.AddIgnoredKey(Shortcut.F7);
        view.AddIgnoredKey(Shortcut.F8);
        view.AddIgnoredKey(Shortcut.F9);

        // autocompletion
        view.AutoCSeparator = (int)'\n';
        view.IsAutoCGetAutoHide = false;
        view.IsAutoCGetIgnoreCase = true;

        // line wrap
        view.WrapMode = (int)Scintilla.Enums.Wrap.word;
        view.WrapVisualFlags = (int)Scintilla.Enums.WrapVisualFlag.end;
      }
      catch (Exception e)
      {
        ExceptionDialog.ShowError(e);
      }

      _parent.Controls.Clear();
      _parent.Controls.Add(view);
      view.Dock = DockStyle.Fill;

    }
  }
}
