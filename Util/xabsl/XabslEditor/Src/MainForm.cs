using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Xml;
using System.Diagnostics;
using System.Collections;

namespace XabslEditor
{
  public partial class MainForm : Form
  {

    XabslBehavior _curBehavior;
    Tab _curTab;
    string _errorMsgs = "";
    ErrorList _errorList;

    List<Tab> _tabs;
    bool _dontUpdateTabs = false;
    
    List<string> _recentBehaviors;
    List<string> _recentOptions;

    string _appPath;

    public MainForm()
    {
      commonConstructor();      
    }

    public MainForm(string openFile)
    {
      commonConstructor();

      openDocument(openFile);
    }

    private void commonConstructor()
    {
      Properties.Settings.Default.Reload();
      _appPath = Application.StartupPath;
      string dir = Directory.GetCurrentDirectory();

      InitializeComponent();

      // one empty tab
      _tabs = new List<Tab>();
      CreateNewTab();

      // set option as default
      tabControlMain.SelectedIndex = 0;

      // nothing to show yet
      enableGraphicalEnviroment(false);

      // any old behaviors?
      string str = Properties.Settings.Default.recentBehaviors;
      string[] recent = str.Split(new char[] { ';' });
      _recentBehaviors = new List<string>();
      int count = Math.Min(recent.Length, 6);

      for (int i = 0; i < count; i++)
      {
        if (!recent[i].Equals(""))
        {
          _recentBehaviors.Insert(0, recent[i]);
        }
      }

      updateRecentBehaviors();

      // load last behavior
      if (_recentBehaviors.Count > 0)
      {
        openBehavior(new DirectoryInfo(_recentBehaviors[0]));
      }

      _recentOptions = new List<string>();

    }

    private void saveConf()
    {
      // save the recent behaviors
      string recent = "";
      foreach (string s in _recentBehaviors)
      {
        recent = s + ";" + recent;
      }
      // remove last semicolon
      if (recent.Length > 0)
      {
        recent = recent.Remove(recent.Length - 1);
      }
      Properties.Settings.Default.recentBehaviors = recent;

      
      Properties.Settings.Default.Save();

    }

    private void updateRecentBehaviors()
    {
      recentBehaviorsToolStripMenuItem.DropDown.Items.Clear();
      for (int i = 0; i < _recentBehaviors.Count; i++)
      {
        ToolStripMenuItem recentItem = new ToolStripMenuItem(_recentBehaviors[i]);
        recentBehaviorsToolStripMenuItem.DropDown.Items.Add(recentItem);
        recentItem.Click += new EventHandler(recentBehaviorItem_Click);
      }
      if (_recentBehaviors.Count > 0)
      {
        recentBehaviorsToolStripMenuItem.Enabled = true;
      }
      else
      {
        recentBehaviorsToolStripMenuItem.Enabled = false;
      }
    }

    private void updateRecentOptions()
    {
      recentOptionsToolStripMenuItem.DropDown.Items.Clear();
      for (int i = 0; i < _recentOptions.Count; i++)
      {
        ToolStripMenuItem recentItem = new ToolStripMenuItem(_recentOptions[i]);
        recentOptionsToolStripMenuItem.DropDown.Items.Add(recentItem);
        recentItem.Click += new EventHandler(recentOptionItem_Click);
      }
      if (_recentOptions.Count > 0)
      {
        recentOptionsToolStripMenuItem.Enabled = true;
      }
      else
      {
        recentOptionsToolStripMenuItem.Enabled = false;
      }
    }

    void recentBehaviorItem_Click(object sender, EventArgs e)
    {
      ToolStripMenuItem item = (ToolStripMenuItem)sender;
      DirectoryInfo behaviorDir = new DirectoryInfo(item.Text);
      openBehavior(behaviorDir);
    }

    void recentOptionItem_Click(object sender, EventArgs e)
    {
      ToolStripMenuItem item = (ToolStripMenuItem)sender;
      string path = _curBehavior.Dir.FullName + "\\" + XabslBehavior.OPTIONS_DIR + "\\" + item.Text;
      resetHistory();

      openDocument(path);
    }

    string getSvgHtml(string svgFile)
    {
      return "<html>\n<body>\n<p>\n"
        + "<object type=\"image/svg+xml\" src=\""
        + svgFile + "\" width=\"5000\" height=\"5000\">\n"
        + "<param name=\"src\" value=\"" + svgFile + "\">\n"
        + "you don't have installed the Adobe SVG Plugin</object>"
        + "</p>\n</body>\n</html>";
    }

    /// <summary>
    /// Will enable or disable all elements of the ui, which are related to the graphical output.
    /// </summary>
    /// <param name="enable">wether to enable (true) or disable (false)</param>
    private void enableGraphicalEnviroment(bool enable)
    {

      if (!enable)
      {
        dropDownButtonOption.Text = "(choose an option)";
        //tabControlMain.SelectedIndex = 0;
      }
      tabControlMain.Enabled = enable;
      buttonUpdate.Enabled = enable;
      updateViewToolStripMenuItem.Enabled = enable;
      webBrowserAgent.Visible = enable;
      webBrowserOption.Visible = enable;
      webBrowserOption.Visible = enable;
      labelEnableGraphView.Visible = !enable;
    }

    /// <summary>
    /// Will enable or disable all elements of the ui, which are related to text editing.
    /// </summary>
    /// <param name="enable">wether to enable (true) or disable (false)</param>
    /// <param name="tab">the tab which is effected</param>
    private void enableTextEnviroment(bool enable, Tab tab)
    {
      tab.view.Enabled = enable;
      insertStateBodyToolStripMenuItem.Enabled = enable;
      commentCodeOutToolStripMenuItem.Enabled = enable;
      uncommentCodeToolStripMenuItem.Enabled = enable;
      autocompletitionToolStripMenuItem.Enabled = enable;
      findToolStripMenuItem.Enabled = enable;
      undoToolStripMenuItem.Enabled = buttonUndo.Enabled = enable;
      redoToolStripMenuItem.Enabled = buttonRedo.Enabled = enable;
    }

    private void openBehavior(DirectoryInfo behaviorDir)
    {
      try
      {
        resetHistory();

        _curBehavior = new XabslBehavior(behaviorDir.FullName);
 
        if(_curBehavior.IsLoaded)
        {
          // add to list of recent behaviors
          List<string> newList = new List<string>();
          newList.Add(_curBehavior.Dir.FullName);
          int i = 1;
          foreach (string s in _recentBehaviors)
          {
            if (i < 6 && s != _curBehavior.Dir.FullName)
            {
              newList.Add(s);
              i++;
            }
          }
          _recentBehaviors = newList;
          updateRecentBehaviors();

          // just for the case...
          saveConf();

          recentBehaviorsToolStripMenuItem.Enabled = true;

          this.Text = "XabslEditor - " + _curBehavior.BehaviorName;

          // close all tabs
          _tabs.Clear();
          tabControlEditor.TabPages.Clear();

          findOptionsAndSymbols();
          toolStripStatusLabel.Text = "";
          dropDownButtonOption.Text = "(choose an option)";
          enableGraphicalEnviroment(false);
          clearCacheToolStripMenuItem.Enabled = true;
          searchInAllOptionsToolStripMenuItem.Enabled = buttonSearchInOptions.Enabled
            = true;
          configurationToolStripMenuItem.Enabled = true;

          CreateNewTab();
        }
      }
      catch (Exception ex)
      {
        ExceptionDialog.ShowError(ex);
      }
    }

    private void openDocument(string path)
    {
      try
      {

        if (_curTab.doc != null && _curTab.doc.CompilationIsRunning)
        {
          _curTab.doc.CancelCompilation();
        }

        _curTab.doc = new XabslDocument(path, _curBehavior, _appPath + "\\" + Properties.Settings.Default.CacheDir);
        _curTab.doc.AddCompilerOutputLine += new EventHandler<StringEventArgs>(_curDocument_AddCompilerOutputLine);
        _curTab.doc.AgentFileReady += new EventHandler<StringEventArgs>(_curDocument_AgentFileReady);
        _curTab.doc.OptionFileReady += new EventHandler<StringEventArgs>(_curDocument_OptionFileReady);
        _curTab.doc.CompilationFinished += new EventHandler(_curDocument_CompilationFinished);
        _curTab.doc.ErrorMessageReceived += new EventHandler<StringEventArgs>(_curDocument_ErrorMessageReceived);
        _curTab.doc.CompilationStarted += new EventHandler(_curDocument_CompilationStarted);
        _curTab.doc.Saved += new EventHandler(_curDocument_Saved);

        toolStripStatusLabel.Text = _curTab.doc.File.FullName;

        _curTab.view.Text = _curTab.doc.Content;
        _curTab.view.SetSavePoint();
        _curTab.view.EmptyUndoBuffer();

        ShowDocument(_curTab, true);

        if (_curTab.doc.IsOption)
        {          
          // add to recent options
          List<string> newList = new List<string>();
          newList.Add(dropDownButtonOption.Text);
          foreach (string s in _recentOptions)
          {
            if (s != dropDownButtonOption.Text)
            {
              newList.Add(s);
            }
          }
          _recentOptions = newList;
          updateRecentOptions();
        }
      }
      catch (Exception ex)
      {
        ExceptionDialog.ShowError(ex);
      }
    }

    private void ShowDocument(Tab theTab, bool updateSvg)
    {

      enableTextEnviroment(true, theTab);

      // adjust buttons
      buttonSave.Enabled = saveCurrentFileToolStripMenuItem.Enabled = !_curTab.wasSaved;
      buttonBack.Enabled = oneOptionBackToolStripMenuItem.Enabled =
        (theTab.backOptions == null || theTab.backOptions.Count == 0) ? false : true;
      buttonForward.Enabled = oneOptionForwardToolStripMenuItem.Enabled =
        (theTab.forwardOptions == null || theTab.forwardOptions.Count == 0) ? false : true;
      
      XabslDocument theDoc = theTab.doc;

      toolStripStatusLabel.Text = theDoc.File.FullName;

      /*
      timerCheckForChangedFile.Enabled = false;
      checkFileChanged(theDoc);
      timerCheckForChangedFile.Enabled = true;
      */

      if (!theDoc.IsOption)
      {
        // set title
        this.Text = "XabslEditor - " + theDoc.File.FullName;
        dropDownButtonOption.Text = "(choose an option)";
        enableGraphicalEnviroment(false);
      }
      else
      {
        enableGraphicalEnviroment(true);
        // set title
        this.Text = "XabslEditor - " + _curBehavior.BehaviorName + ": " + theDoc.RelativeOptionPath;

        dropDownButtonOption.Text = theDoc.RelativeOptionPath;
        if (updateSvg)
        {
          // get the needed files for graphical output
          theDoc.RequestSvgFiles(false);
        }
      }

      // set tab title
      tabControlEditor.SelectedTab.Text = theTab.wasSaved ? theDoc.File.Name : theDoc.File.Name + " *";

      
      // mark errors
      
      // clear old errors first
      theTab.view.StartStyling(0, Helpers.SCI_INDICS_MASK);
      theTab.view.SetStyling(theTab.view.Text.Length, 0);
      theTab.view.Colourise(0, theTab.view.Length - 1);

      if (_errorList != null && _errorList.Errors != null)
      {
        string[] seps = new string[] { "\r\n", "\n", "\r" };
        string[] lines = theTab.view.Text.Split(seps, StringSplitOptions.None);
              
        foreach (ErrorEntry entry in _errorList.Errors)
        {
          // the same document?
          if (entry.file.Equals(theDoc.File.FullName, StringComparison.InvariantCultureIgnoreCase))
          {
            int posFromLine = theTab.view.PositionFromLine(entry.line-1);
       
            try
            {
              if (entry.line > -1 && entry.line < lines.Length)
              {
                string theLine = lines[entry.line - 1];

                int startMark = 0;
                if (entry.errorSample != null)
                {
                  startMark = theLine.IndexOf(entry.errorSample);
                }
                theTab.view.StartStyling(posFromLine + startMark, Helpers.SCI_INDICS_MASK);

                int length = 0;
                if (entry.errorSample == null)
                {
                  length = theLine.Length;
                }
                else
                {
                  length = entry.errorSample.Length;
                }
                theTab.view.SetStyling(length, Helpers.SCI_INDIC0_MASK);

                theTab.view.SetIndicStyle(0, (int)Scintilla.Enums.IndicatorStyle.squiggle);
                theTab.view.SetIndicFore(0, 0x0000ff);
              }
            }
            catch (Exception ex)
            {
              ExceptionDialog.ShowWarning(ex);
            }
          }
        }
      }

    }

    private void ShowTab(int index)
    {
      if (index < _tabs.Count && index >= 0)
      {

        if (_curTab != null && _curTab.doc != null
          && _curTab.doc.CompilationIsRunning)
        {
          _curTab.doc.CancelCompilation();
        }

        _curTab = _tabs[index];

        tabControlEditor.SelectedIndex = index;

        if (_curTab.doc == null)
        {
          // is empty
          tabControlEditor.TabPages[index].Text = "empty";
          _curTab.view.EmptyUndoBuffer();
          _curTab.view.SetSavePoint();
          dropDownButtonOption.Text = "(choose an option)";
          toolStripStatusLabel.Text = "";
          enableGraphicalEnviroment(false);
          enableTextEnviroment(false, _curTab);
        }
        else
        {
          ShowDocument(_curTab, true);
        }
      }
    }

    private void CreateNewTab()
    {
      tabControlEditor.TabPages.Add("empty");
      Tab t = new Tab(null, tabControlEditor.TabPages[tabControlEditor.TabCount-1],
        _appPath + "\\" + Properties.Settings.Default.ScintillaXabslFile);
      t.view.SavePointLeft += new Scintilla.SavePointLeftHandler(scintillaControlInput_SavePointLeft);
      t.view.SavePointReached += new Scintilla.SavePointReachedHandler(scintillaControlInput_SavePointReached);


      _tabs.Add(t);
      tabControlEditor.SelectTab(tabControlEditor.TabPages.Count - 1);
      ShowTab(_tabs.Count - 1);
    }
    
    private void CloseTab(int index)
    {
      if (index < _tabs.Count && index >= 0)
      {
        if (_tabs[index].doc != null && _tabs[index].doc.CompilationIsRunning)
        {
          _tabs[index].doc.CancelCompilation();
        }

        // check for saves
        if (checkSaved()) return;

        tabControlEditor.TabPages.RemoveAt(index);
        _tabs.RemoveAt(index);

        // create an empty default-page if neccessary
        if (_tabs.Count == 0)
        {
          CreateNewTab();
        }        
      }
    }

    public void MakeSureDocumentIsVisible(XabslDocument doc)
    {
      if (_curTab.doc == null || !_curTab.doc.Equals(doc))
      {
        int foundAt = -1;
        int i = 0;
        foreach (Tab tab in _tabs)
        {
          if (tab.doc != null && tab.doc.Equals(doc))
          {
            foundAt = i;
            break;
          }
          i++;
        }
        if (foundAt >= 0)
        {
          ShowTab(foundAt);
        }
        else
        {
          CreateNewTab();
          openDocument(doc.File.FullName);
        }
      }
    }

    void _curDocument_Saved(object sender, EventArgs e)
    {
      toolStripStatusLabel.Text = _curTab.doc.File.FullName + ": saved at "
              + System.DateTime.Now.ToShortTimeString();
      _curTab.view.SetSavePoint();
    }

    void _curDocument_CompilationStarted(object sender, EventArgs e)
    {
      if ((XabslDocument)sender != _curTab.doc) return;

      _errorMsgs = "";

      textBoxCompiler.Text = "compilation started...\r\n";
      webBrowserOption.DocumentText = webBrowserAgent.DocumentText =
        "<html><body><p>compiling...</p></body></html>";
      tabPageErrors.ImageIndex = -1;
      treeViewErrors.Nodes.Clear();
    }

    void _curDocument_ErrorMessageReceived(object sender, StringEventArgs e)
    {
      if ((XabslDocument)sender != _curTab.doc) return;

      webBrowserOption.DocumentText = webBrowserAgent.DocumentText =
        "<html><body><p>error: " + e.Text + "</p></body></html>";
    }

    void _curDocument_CompilationFinished(object sender, EventArgs e)
    {
      if ((XabslDocument)sender != _curTab.doc) return;

      buttonUpdate.Enabled = true;
      updateViewToolStripMenuItem.Enabled = true;

      _errorList = new ErrorList(_errorMsgs);
      if (_errorList != null && _errorList.Errors == null)
      {
        _errorList = null;
      }

      if (_errorList != null && _errorList.Errors.Count > 0)
      {
        tabPageErrors.ImageIndex = 0;
        treeViewErrors.DoubleClick += new EventHandler(treeViewErrors_DoubleClick);
        treeViewErrors.KeyPress += new KeyPressEventHandler(treeViewErrors_KeyPress);
        foreach (ErrorEntry error in _errorList.Errors)
        {
          // add to tree-view
          TreeNode added = treeViewErrors.Nodes.Add(error.errorDescription);
          added.Nodes.Add(error.errorNumber);
          added.Nodes.Add(error.file);
          added.Nodes.Add("line: " + error.line);
        }
      }
      else
      {
        // hurray, no errors
        TreeNode node = treeViewErrors.Nodes.Add("no errors");
        node.ImageIndex = 1;
        node.SelectedImageIndex = 1;
        tabPageErrors.ImageIndex = 1;
        treeViewErrors.DoubleClick -= this.treeViewErrors_DoubleClick;
        treeViewErrors.KeyPress -= this.treeViewErrors_KeyPress;
      }

      // reinit view (e.g. if errors occured)
      ShowDocument(_curTab, false);
    }

    void treeViewErrors_KeyPress(object sender, KeyPressEventArgs e)
    {
      if (e.KeyChar == '\r' || e.KeyChar == '\n')
      {
        showSelectedError();
      }
    }

    private void showSelectedError()
    {
      TreeNode node = treeViewErrors.SelectedNode;
      TreeNode errorNode = node;
      // sub-node?
      if (node.Level == 1)
      {
        errorNode = node.Parent;
      }

      int i = errorNode.Index;

      ErrorEntry error = _errorList.Errors[i];
      // show
      XabslDocument doc = new XabslDocument(error.file, _curBehavior,
        _appPath + "\\" + Properties.Settings.Default.CacheDir);
      MakeSureDocumentIsVisible(doc);
      // goto line
      if (error.line > -1 && error.line < _curTab.view.LineCount)
      {
        int pos = _curTab.view.PositionFromLine(error.line-1);
        _curTab.view.GotoPos(pos);
        _curTab.view.SetSel(pos, pos + _curTab.view.GetLine(error.line-1).Length - 1);
      }
      else
      {
        _curTab.view.SetSel(0, 0);
      }
    }

    void treeViewErrors_DoubleClick(object sender, EventArgs e)
    {
      showSelectedError();
    }
    
    void _curDocument_OptionFileReady(object sender, StringEventArgs e)
    {
      if ((XabslDocument)sender != _curTab.doc) return;

      webBrowserOption.DocumentText = getSvgHtml(e.Text);
    }

    void _curDocument_AgentFileReady(object sender, StringEventArgs e)
    {
      if ((XabslDocument)sender != _curTab.doc) return;

      webBrowserAgent.DocumentText = getSvgHtml(e.Text);
    }

    void _curDocument_AddCompilerOutputLine(object sender, StringEventArgs e)
    {
      if ((XabslDocument)sender != _curTab.doc) return;

      _errorMsgs += "\n" + e.Text;
      textBoxCompiler.AppendText(e.Text + "\r\n");
    }

    private void resetHistory()
    {
      buttonForward.Enabled = oneOptionForwardToolStripMenuItem.Enabled = false;
      buttonBack.Enabled = oneOptionBackToolStripMenuItem.Enabled = false;

      _curTab.backOptions.Clear();
      _curTab.forwardOptions.Clear();
    }

    private void openBehaviorToolStripMenuItem_Click(object sender, EventArgs e)
    {
      checkSaved();

      DialogResult dlgResult = folderBrowserDialog1.ShowDialog();
      if (dlgResult == DialogResult.OK)
      {
        DirectoryInfo behaviorDir = new DirectoryInfo(folderBrowserDialog1.SelectedPath);
        openBehavior(behaviorDir);
      }
    }

    private void findOptionsAndSymbols()
    {
      try
      {
        // AGENT.XABSL AND SYMBOLS //
        dropDownButtonAgent.DropDownItems.Clear();
        if (_curBehavior != null)
        {
          dropDownButtonAgent.DropDownItems.Add(new ToolStripMenuItem(_curBehavior.Agent.Name,
            null, new EventHandler(optionItem_Activate), _curBehavior.Agent.FullName));

          ToolStripMenuItem symbols = new ToolStripMenuItem("Symbols");
          _curBehavior.FindAvailableSymbols();
          foreach (FileInfo f in _curBehavior.Symbols)
          {
            ToolStripMenuItem item = new ToolStripMenuItem(f.Name, null,
              new EventHandler(optionItem_Activate), f.FullName);
            symbols.DropDownItems.Add(item);
          }

          dropDownButtonAgent.DropDownItems.Add(symbols);
        }

        // OPTIONS //

        // delete old entries
        dropDownButtonOption.DropDownItems.Clear();

        if (_curBehavior.Options != null)
        {
          _curBehavior.FindAvailableOptions();
        }

        foreach (FileInfo f in _curBehavior.Options)
        {
          XabslDocument doc = new XabslDocument(f.FullName, _curBehavior,
            _appPath + "\\" + Properties.Settings.Default.CacheDir);
          if (doc.ClassLoadedSuccessfully)
          {
            if (doc.IsTopOption)
            {
              dropDownButtonOption.DropDownItems.Add(new ToolStripMenuItem(doc.OptionName + XabslDocument.XabslEnding, null,
                  new EventHandler(optionItem_Activate), doc.FullPath.FullName + "\\" + doc.OptionName + XabslDocument.XabslEnding));
            }
            else
            {
              ToolStripMenuItem item = new ToolStripMenuItem(doc.OptionName + XabslDocument.XabslEnding, null,
                  new EventHandler(optionItem_Activate), doc.FullPath.FullName + "\\" + doc.OptionName + XabslDocument.XabslEnding);
              ToolStripMenuItem parent = insertOption(doc.FullPath);
              parent.DropDownItems.Add(item);
            }
          }
        }
      }
      catch (Exception exc)
      {
        ExceptionDialog.ShowError(exc);
      }
    }

    private ToolStripMenuItem insertOption(DirectoryInfo dir)
    {
      // find the right menu to insert the option
      ToolStripMenuItem parent = null;
      if (dir.Name != XabslBehavior.OPTIONS_DIR)
      {
        parent = insertOption(dir.Parent);
        if (parent == null)
        {
          parent = (ToolStripMenuItem)dropDownButtonOption.DropDownItems[dir.Name];
          if (parent == null)
          {
            parent = new ToolStripMenuItem(dir.Name, null, null, dir.Name);
            dropDownButtonOption.DropDownItems.Add(parent);
          }
          return (ToolStripMenuItem)dropDownButtonOption.DropDownItems[dir.Name];
        }
        else
        {
          ToolStripMenuItem item = (ToolStripMenuItem)parent.DropDownItems[dir.Name];
          if (item == null)
          {
            item = new ToolStripMenuItem(dir.Name, null, null, dir.Name);
            parent.DropDownItems.Add(item);
          }
          return item;
        }
      }
      return null;
    }

    /** returns true, if cancelation was requested */
    private bool checkSaved(Tab tab)
    {
      if(tab == null) return false;

      XabslDocument doc = tab.doc;

      bool curDocChanged = false;
      if (doc != null && _curTab.doc == doc && !tab.wasSaved)
        curDocChanged = true;

      if ((doc != null && !doc.ChangesSaved) || curDocChanged)
      {
        DialogResult result = MessageBox.Show(doc.File.FullName + "\n\nFile not saved yet, do you want to save it now?", "file not saved", MessageBoxButtons.YesNoCancel);
        if (result == DialogResult.Yes)
        {
          // save the file
          if (curDocChanged)
          {
            doc.Content = _curTab.view.Text;
          }
          doc.SaveChanges();
        }
        else if (result == DialogResult.Cancel)
        {
          return true;
        }
      }
      return false;
    }

    private bool checkSaved()
    {
      return checkSaved(_curTab);
    }

    /** checks wether a file has been changed by another program */
    private void checkFileChanged(Tab t)
    {
      if(t == null) return;

      XabslDocument doc = t.doc;

      if (doc != null && doc.FileChangedOutside)
      {
        // file has been changed
        DialogResult result = MessageBox.Show(doc.File.FullName
          + "\n\n The file has been changed by another program. Shall the file be reloaded?",
          "file changed outside of program",
          MessageBoxButtons.YesNo);
        if (result == DialogResult.Yes)
        {
          doc.ReloadFileContent();
          t.view.Text = doc.Content;
        }
      }
    }

    void optionItem_Activate(object sender, EventArgs e)
    {
      try
      {

        if (checkSaved()) return; ;

        resetHistory();

        ToolStripItem item = (ToolStripItem) sender;
        
        openDocument(item.Name);
        
      }
      catch (Exception exc)
      {
        ExceptionDialog.ShowError(exc);
      }
    }

    private void configurationToolStripMenuItem_Click(object sender, EventArgs e)
    {
      // show conf-dialog
      if (_curBehavior != null)
      {
        _curBehavior.ShowConfigurationDlg();
      }
      else
      {
        MessageBox.Show("no behavior opened yet!");
      }
    }

    private void exitToolStripMenuItem_Click(object sender, EventArgs e)
    {
      this.Close();
    }

    private void autocompletitionToolStripMenuItem_Click(object sender, EventArgs e)
    {
      
      if (_curTab.doc.ScintillaAutoCList == null)
      {
        _curTab.view.AutoCShow(0, "*no autocompletion available*");
        return;
      }

      string text = _curTab.view.Text;
      // get the already typed word
      int posEnd = _curTab.view.CurrentPos;
      int posBegin = posEnd;
      while (posBegin > 0 &&
        (Char.IsLetterOrDigit(text[posBegin - 1]) || text[posBegin - 1] == '_' || text[posBegin - 1] == '.'))
      {
        posBegin--;
      }

      string word = text.Substring(posBegin, posEnd - posBegin);
      
      _curTab.view.AutoCShow(word.Length, _curTab.doc.ScintillaAutoCList);
    }

    private void updateViewToolStripMenuItem_Click(object sender, EventArgs e)
    {
      // just to be sure
      if (!buttonUpdate.Enabled) return;

      if (_curTab.doc.CompilationIsRunning)
      {
        DialogResult r = MessageBox.Show("I'm still running the update, shall I stop it now?", "update is running", MessageBoxButtons.YesNo);
        if (r.Equals(DialogResult.Yes))
        {
          _curTab.doc.CancelCompilation();
          MessageBox.Show("you have to restart the update now");
        }
      }
      else
      {
        _curTab.doc.Content = _curTab.view.Text;
        _curTab.view.SetSavePoint();
        foreach (Tab t in _tabs)
        {
          checkFileChanged(t);
          if (t.doc != null)
          {
            t.doc.SaveChanges();
          }
        }
        _curTab.doc.RequestSvgFiles(true);
      }
    }

    private void buttonNewFile_Click(object sender, EventArgs e)
    {

      if (_curBehavior != null)
      {
        saveFileDialog.InitialDirectory = _curBehavior.Dir.FullName;
      }

      if(checkSaved()) return;

      if (saveFileDialog.ShowDialog() == DialogResult.OK)
      {
        try
        {
          resetHistory();

          // create this file
          FileStream stream =  File.Create(saveFileDialog.FileName);
          stream.Close();

          // update options-menu
          if (_curBehavior != null)
          {
            findOptionsAndSymbols();
          }

          openDocument(saveFileDialog.FileName);

        }
        catch (Exception ex)
        {
          ExceptionDialog.ShowError(ex);
        }
      }
    }
             
    private void buttonSave_Click(object sender, EventArgs e)
    {
      try
      {

        // nothing todo
        if (_curTab.wasSaved) return;

        _curTab.doc.Content = _curTab.view.Text;
        _curTab.doc.SaveChanges();
      }
      catch (Exception exc)
      {
        ExceptionDialog.ShowError(exc);
      }
    }

    private void buttonOpen_Click(object sender, EventArgs e)
    {

      if (_curBehavior != null)
      {
        openFileDialog.InitialDirectory = _curBehavior.Dir.FullName;
      }

      if (checkSaved()) return;

      DialogResult result = openFileDialog.ShowDialog();

      if (result == DialogResult.OK)
      {
        try
        {

          openDocument(openFileDialog.FileName);
          resetHistory();
        }
        catch (IOException ioE)
        {
          ExceptionDialog.ShowError(ioE);
        }
      }
    }

    private void buttonUndo_Click(object sender, EventArgs e)
    {
      // simply delegate the responsibility to Scintilla
      _curTab.view.Undo();
    }

    private void buttonRedo_Click(object sender, EventArgs e)
    {
      // simply delegate the responsibility to Scintilla
      _curTab.view.Redo();
    }

    private void webBrowserOption_Navigating(object sender, WebBrowserNavigatingEventArgs e)
    {
      // is option?
      if(e.Url.AbsoluteUri.EndsWith(".optdummy"))
      {
        e.Cancel = true; 
        
        if (checkSaved()) return;

        int begin = e.Url.AbsoluteUri.LastIndexOf('/');
        int length = e.Url.AbsoluteUri.Length - begin - 10;

        if (sender == webBrowserOption)
        {
          // save old option in list
          _curTab.backOptions.Insert(0, _curTab.doc.File.FullName);
          buttonBack.Enabled = oneOptionBackToolStripMenuItem.Enabled = true;

          // information about forward options is invalid
          _curTab.forwardOptions.Clear();
          buttonForward.Enabled = oneOptionForwardToolStripMenuItem.Enabled = false;
        }
        else
        {
          // start over again
          resetHistory();
        }

        // find the option in list
        string option = e.Url.AbsoluteUri.Substring(begin + 1, length) + XabslDocument.XabslEnding;
        // find the corresponding menu-item
        ToolStripMenuItem mi_option = null;
        foreach (ToolStripMenuItem mi in dropDownButtonOption.DropDown.Items)
        {
          if(mi.DropDown.Items.Count > 0)
          {
            // has sub-items
            foreach (ToolStripMenuItem m_sub in mi.DropDown.Items)
            {
              if (m_sub.Text == option)
              {
                mi_option = m_sub;
                break;
              }              
            }
            if (mi_option != null)
            {
              break;
            }
          }
          else if (mi.Text == option)
          {
            mi_option = mi;
            break;
          }
        }

        // open option
        if (mi_option != null)
        {
          string newPath = _curBehavior.Dir.FullName + "\\" + XabslBehavior.OPTIONS_DIR + "\\";
          if (mi_option.OwnerItem != dropDownButtonOption)
          {
            newPath += mi_option.OwnerItem.Text + "\\";
          }

          newPath += mi_option.Text;
          openDocument(newPath);
        }
      }
      // is state?
      else if (e.Url.AbsoluteUri.EndsWith(".statedummy"))
      {
        e.Cancel = true;

        int begin = e.Url.AbsoluteUri.LastIndexOf('/');
        int length = e.Url.AbsoluteUri.Length - begin - 12;

        string state = e.Url.AbsoluteUri.Substring(begin + 1, length);
        
        int oldPos = _curTab.view.CurrentPos;

        // find state (by use of regular expressions)
        // begin at start
        _curTab.view.GotoPos(0);
        _curTab.view.SearchAnchor();

        // find
        int args = (int) Scintilla.Enums.FindOption.regexp;
        string searchText = "state[ \t\r\n]+" + state + "[ \t\r\n{]*$";
        int pos = _curTab.view.SearchNext(args, searchText);
        if (pos > -1)
        {
          _curTab.view.GotoPos(_curTab.view.Length);
          int line = _curTab.view.LineFromPosition(pos);
          int startPos = _curTab.view.PositionFromLine(line);
          int endPos = _curTab.view.LineEndPosition(line);
          _curTab.view.SetSel(startPos, endPos);
          _curTab.view.GrabFocus();

        }
        else
        {
          // go to old pos
          _curTab.view.GotoPos(oldPos);
        }


      }
    }

    private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
    {
      foreach (Tab tab in _tabs)
      {
        if (checkSaved(tab))
        {
          e.Cancel = true;
          return;
        }
      }
      

      saveConf();
     
      // everthing finished?
      if (_curTab != null && _curTab.doc != null)
      {
        _curTab.doc.CancelCompilation();
      }
    }

    private void findToolStripMenuItem_Click(object sender, EventArgs e)
    {
      Find findDlg = new Find(_curTab.view);
      findDlg.Show(this);
    }


    private void toolStripButtonSearchInOptions_Click(object sender, EventArgs e)
    {
      if (_curBehavior == null) return;

      try
      {
        SearchInOptions dlg = new SearchInOptions(this, _curBehavior, _curTab.view.SelText);
        dlg.Show();
      }
      catch (Exception ex)
      {
        ExceptionDialog.ShowError(ex);
      }
    }

    private void commentCodeOutToolStripMenuItem_Click(object sender, EventArgs e)
    {
      // get selected text
      string selected = _curTab.view.SelText;

      if (selected == null || selected.Equals(""))
      {
        // nothing todo
        return;
      }

      string newText = "";

      // every line
      string[] split = selected.Split(new char[] { '\n'});
      for (int i = 0; i < split.Length - 1; i++)
      {
        newText = newText + "//" + split[i] + "\n";
      }

      if(split.Length > 0)
      {
        newText = newText + "//" + split[split.Length-1];
      }

      // replace text
      _curTab.view.ReplaceSel(newText);
    }
 
    private void uncommentCodeToolStripMenuItem_Click(object sender, EventArgs e)
    {
      // get selected text
      string selected = _curTab.view.SelText;

      if (selected == null || selected.Equals(""))
      {
        // nothing todo
        return;
      }

      string newText = "";

      // every line
      string[] split = selected.Split(new char[] { '\n' });
      for (int i = 0; i < split.Length - 1; i++)
      {
        if (split[i].StartsWith("//"))
        {
          newText = newText + split[i].Substring(2) + "\n";
        }
        else
        {
          newText = newText + split[i] + "\n";
        }
      }

      if (split.Length > 0)
      {
        if (split[split.Length - 1].StartsWith("//"))
        {
          newText = newText + split[split.Length - 1].Substring(2);
        }
        else
        {
          newText = newText + split[split.Length - 1];
        }
      }

      // replace text
      _curTab.view.ReplaceSel(newText);
    }

    private void dummyOptionToolStripMenuItem_Click(object sender, EventArgs e)
    {
      dropDownButtonOption.ShowDropDown();
    }

    private void dummyTextToolStripMenuItem_Click(object sender, EventArgs e)
    {
      _curTab.view.GrabFocus();
    }

    private void dummyViewToolStripMenuItem_Click(object sender, EventArgs e)
    {
      tabControlMain.Select();
    }
    
    private void dummyRecentOptionToolStripMenuItem_Click(object sender, EventArgs e)
    {
      fileToolStripMenuItem.ShowDropDown();
      recentOptionsToolStripMenuItem.ShowDropDown();
    }
        
    private void buttonBack_Click(object sender, EventArgs e)
    {
      if(_curTab.backOptions.Count > 0)
      {
        string newOption = _curTab.backOptions[0];
        _curTab.forwardOptions.Insert(0, _curTab.doc.File.FullName);
        buttonForward.Enabled = oneOptionForwardToolStripMenuItem.Enabled = true;

        _curTab.backOptions.RemoveAt(0);
        if (_curTab.backOptions.Count < 1)
        {
          buttonBack.Enabled = oneOptionBackToolStripMenuItem.Enabled = false;
        }

        openDocument(newOption);
      }
    }

    private void buttonForward_Click(object sender, EventArgs e)
    {
      if (_curTab.forwardOptions.Count > 0)
      {
        string newOption = _curTab.forwardOptions[0];
        _curTab.backOptions.Insert(0, _curTab.doc.File.FullName);
        buttonBack.Enabled = oneOptionBackToolStripMenuItem.Enabled = true;

        _curTab.forwardOptions.RemoveAt(0);
        if (_curTab.forwardOptions.Count < 1)
        {
          buttonForward.Enabled = oneOptionForwardToolStripMenuItem.Enabled = false;
        }

        openDocument(newOption);
      }

    }
        
    private void scintillaControlInput_SavePointLeft(Scintilla.ScintillaControl pSender)
    {
      buttonSave.Enabled = saveCurrentFileToolStripMenuItem.Enabled = true;
      _curTab.wasSaved = false;
      if (tabControlEditor.SelectedTab != null && _curTab != null && _curTab.doc != null)
      {
        tabControlEditor.SelectedTab.Text = _curTab.doc.File.Name + " *";
      }
    }

    private void scintillaControlInput_SavePointReached(Scintilla.ScintillaControl pSender)
    {
      buttonSave.Enabled = saveCurrentFileToolStripMenuItem.Enabled = false;
      _curTab.wasSaved = true;
      if (tabControlEditor.SelectedTab != null && _curTab != null && _curTab.doc != null)
      {
        tabControlEditor.SelectedTab.Text = _curTab.doc.File.Name;
      }
    }

    private void insertStateBodyToolStripMenuItem_Click(object sender, EventArgs e)
    {
      _curTab.view.InsertText(_curTab.view.CurrentPos,
        "  state name {\n    decision {\n      stay;\n    }\n    action {\n      \n    }\n  }\n");
    }

    private void clearCacheToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (_curBehavior == null) return;

      try
      {
        bool deleted = false;
        string deletedDir = "";

        // delete all svg- and dot -files
        DirectoryInfo confDir =  new DirectoryInfo(_appPath + "\\" + Properties.Settings.Default.CacheDir
              + _curBehavior.BehaviorName + "\\");
        if (confDir.Exists)
        {
          confDir.Delete(true);
          deleted = true;
          deletedDir += "* svg- and dot-files\n";
        }

        // delete xml-Data
        DirectoryInfo buildDir = new DirectoryInfo(_curBehavior.BuildDir
            + "\\Options");

        if (buildDir.Exists)
        {
          buildDir.Delete(true);
          deleted = true;
          deletedDir += "* xml-files\n";

          // "touch" agent in order to force rebuild next time
          _curBehavior.Agent.LastWriteTime = DateTime.Now;

        }

        if(deleted)
          MessageBox.Show("Cache successfully cleared:\n" + deletedDir);
        else
          MessageBox.Show("Cache was already clean");
      }
      catch (Exception ex)
      {
        ExceptionDialog.ShowError(ex);
      }
    }

    private void newTabToolStripMenuItem_Click(object sender, EventArgs e)
    {
      CreateNewTab();
      _dontUpdateTabs = true;
      ShowTab(_tabs.Count - 1);
      _dontUpdateTabs = false;
    }

    private void tabControlEditor_SelectedIndexChanged(object sender, EventArgs e)
    {
      // prevent endless loop, because ShowTab will cause this event
      if (_dontUpdateTabs) return;

      _dontUpdateTabs = true;
      ShowTab(tabControlEditor.SelectedIndex);
      _dontUpdateTabs = false;
    }

    private void closeTabToolStripMenuItem_Click(object sender, EventArgs e)
    {
      CloseTab(tabControlEditor.SelectedIndex);
    }

    private void timerCheckForChangedFile_Tick(object sender, EventArgs e)
    {
      timerCheckForChangedFile.Enabled = false;
      foreach (Tab t in _tabs)
      {
        checkFileChanged(t);
      }
      timerCheckForChangedFile.Enabled = true;
    }

    private void MainForm_Load(object sender, EventArgs e)
    {
    }

    private void infoToolStripMenuItem_Click(object sender, EventArgs e)
    {
      AboutXablsEditor dlg = new AboutXablsEditor();
      dlg.ShowDialog(this);
      
    }
    
  }
}