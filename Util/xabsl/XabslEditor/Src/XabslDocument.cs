using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Xml;
using System.Collections;
using System.Diagnostics;

namespace XabslEditor
{

  #region helper-classes
  public class Transition
  {
    public string description;
    public string from;
    public string to;
  }

  public class UnderlyingElement
  {
    public string name;
    public bool isOption;
  }

  /** used to compare string */
  public class ScintillaCompare : Comparer<string>
  {
    public override int Compare(string x, string y)
    {
      // sort '_' after normal characters
      x = x.Replace('_', (char)255);
      y = y.Replace('_', (char)255);
      return StringComparer.InvariantCulture.Compare(x, y);
    }
  }

  public class StringEventArgs : EventArgs
  {
    private string _text;

    public string Text
    {
      get { return _text; }
      set { _text = value; }
    }

    public StringEventArgs(string text)
    {
      _text = text;
    }
  }

  #endregion

  public class XabslDocument
  {
    /// <summary>
    /// The file ending for XABSL-files
    /// </summary>
    public const string XabslEnding = ".xabsl";

    private FileInfo _curFile;

    /// <summary>
    /// The file which belongs to the document.
    /// </summary>
    public FileInfo File
    {
      get {return _curFile; }
    }

    DirectoryInfo _resourceDir;
    XabslBehavior _behavior;
    System.ComponentModel.BackgroundWorker backgroundWorkerUpdate;

    private string _content;

    /// <summary>
    /// The textual content of the document.
    /// </summary>
    public string Content
    {
      get { return _content; }
      set 
      { 
        _content = value;
        _changesSaved = false;
      }
    }

    private bool _isOption;

    /// <summary>
    /// Returns wether this document is an option inside the XABSL-behavior.
    /// </summary>
    public bool IsOption
    {
      get { return _isOption; }
    }

    private string _optionName;

    /// <summary>
    /// If the document is an option, this returns the corresponding name.
    /// </summary>
    public string OptionName
    {
      get { return _optionName; }
    }

    /// <summary>
    /// Path from behavior directory to the file containing the option.
    /// </summary>
    public string RelativeOptionPath
    {
      get 
      {
        if (_isOption)
        {
          if (_curFile.Directory.Name == XabslBehavior.OPTIONS_DIR)
          {
            // top-dir
            return _optionName + XabslEnding;
          }
          else
          {
            // subdir
            return _curFile.Directory.Name + "\\" + _optionName + XabslEnding;
          }
        }
        else
        {
          return _curFile.Name;
        }
      }
    }

    /// <summary>
    /// If the document is an option, returns wether the corresponding file is part of the behavior top-directory.
    /// </summary>
    public bool IsTopOption
    {
      get { return (_curFile.Directory.Name == XabslBehavior.OPTIONS_DIR); }
    }

    /// <summary>
    /// Get the parent directory for this option.
    /// </summary>
    public string OptionParentDir
    {
      get 
      {
        if (_curFile.Directory.Name == XabslBehavior.OPTIONS_DIR)
        {
          return "";
        }
        else
        {
          return _curFile.Directory.Name;
        }
      }
    }

    /// <summary>
    /// The path of the documents directory
    /// </summary>
    public DirectoryInfo FullPath
    {
      get
      {
        return _curFile.Directory;
      }
    }

    private string _autoCList;

    /// <summary>
    /// Every line contains an entry which can be used by autocompletion.
    /// The information comes from the compiling process.
    /// </summary>
	  public string ScintillaAutoCList
	  {
		  get { return _autoCList;}
	  }

    private bool _classLoadedSuccessfully;

    /// <summary>
    /// Gets wether the class has successfully finished loading
    /// </summary>
    public bool ClassLoadedSuccessfully
    {
      get { return _classLoadedSuccessfully; }
    }

    private bool _changesSaved;

    /// <summary>
    /// Gets wether the changes made in the document have been saved.
    /// </summary>
    public bool ChangesSaved
    {
      get { return _changesSaved; }
    }

    private DateTime _timeOfLastSave;

    /// <summary>
    /// Gets the time when the file was last saved.
    /// </summary>
    public DateTime TimeOfLastSave
    {
      get { return _timeOfLastSave; }    
    }
	
    /// <summary>
    /// Returns true if a compilation process is started and has not terminated yet.
    /// </summary>
    public bool CompilationIsRunning
    {
      get { return backgroundWorkerUpdate.IsBusy; }
    }

    /// <summary>
    /// Gets wether the file to which the document belongs has been saved by another process.
    /// </summary>
    public bool FileChangedOutside
    {
      get 
      { 
        _curFile.Refresh(); 
        return _curFile.LastWriteTime.CompareTo(_timeOfLastSave) > 0 ? true : false;
      }
    }

    /// <summary>
    /// Is fired when the requested SVG-file for the option view was created.
    /// </summary>
    public event EventHandler<StringEventArgs> OptionFileReady;
    /// <summary>
    /// Is fired when the requested SVG-file for the agent-like view was created.
    /// </summary>
    public event EventHandler<StringEventArgs> AgentFileReady;
    /// <summary>
    /// Will be fired when the compiler has written a line to stdout or stderr.
    /// </summary>
    public event EventHandler<StringEventArgs> AddCompilerOutputLine;
    /// <summary>
    /// Is fired when an error occured and an error message should be displayed to the user.
    /// </summary>
    public event EventHandler<StringEventArgs> ErrorMessageReceived;
    /// <summary>
    /// Is fired when the compilation process has started.
    /// </summary>
    public event EventHandler CompilationStarted;
    /// <summary>
    /// Is fired when the compilation process has finished.
    /// </summary>
    public event EventHandler CompilationFinished;
    /// <summary>
    /// Is fired when the file is saved.
    /// </summary>
    public event EventHandler Saved;

    /// <summary>
    /// Public constructor for the class XabslDocument.
    /// </summary>
    /// <param name="path">Path to the file which the document shall represent</param>
    /// <param name="behavior">Path to the directory where the corresponding XABSL-behavior is.</param>
    /// <param name="resourceDir">Path to the directory where to put ressource files (SVG and etc.) into.</param>
    public XabslDocument(string path, XabslBehavior behavior,string resourceDir)
    {
      try
      {

        backgroundWorkerUpdate = new System.ComponentModel.BackgroundWorker();
        backgroundWorkerUpdate.DoWork += new System.ComponentModel.DoWorkEventHandler(backgroundWorkerUpdate_DoWork);
        backgroundWorkerUpdate.ProgressChanged += new System.ComponentModel.ProgressChangedEventHandler(backgroundWorkerUpdate_ProgressChanged);
        backgroundWorkerUpdate.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(backgroundWorkerUpdate_RunWorkerCompleted);
        backgroundWorkerUpdate.WorkerReportsProgress = true;
        backgroundWorkerUpdate.WorkerSupportsCancellation = true;

        if (resourceDir != null)
        {
          _resourceDir = new DirectoryInfo(resourceDir);
        }
        else
        {
          _resourceDir = null;
        }
        _behavior = behavior;

        _curFile = new FileInfo(path);
        
        // read content of the file
        StreamReader reader = new StreamReader(_curFile.FullName);
        _content = reader.ReadToEnd();
        reader.Close();
        _changesSaved = true;
        _timeOfLastSave = _curFile.LastWriteTime;

        bool insideBehavior = false;
        foreach (FileInfo f in _behavior.Options)
        {
          if (f.FullName == _curFile.FullName)
          {
            insideBehavior = true;
            break;
          }
        }

        if (insideBehavior)
        {
          // file inside the behavior
          _isOption = true;
          _optionName = _curFile.Name.Substring(0, _curFile.Name.Length - XabslEnding.Length); 
        }
        else
        {
          _isOption = false;
        }

        _classLoadedSuccessfully = true;
      }
      catch (Exception ex)
      {
        _classLoadedSuccessfully = false;
        ExceptionDialog.ShowError(ex);        
      }
    }
    
    void backgroundWorkerUpdate_RunWorkerCompleted(object sender, System.ComponentModel.RunWorkerCompletedEventArgs e)
    {
      initAutoCompletion(_behavior.ApiFile);
      CompilationFinished(this, null);
    }

    void backgroundWorkerUpdate_ProgressChanged(object sender, System.ComponentModel.ProgressChangedEventArgs e)
    {
      if (e.ProgressPercentage == 0)
      {
        CompilationStarted(this, new EventArgs());
      }
      else if (e.ProgressPercentage == 1)
      {
        string compmsg = (string)e.UserState;
        AddCompilerOutputLine(this, new StringEventArgs(compmsg));
      }
      else if (e.ProgressPercentage == 2)
      {
        AddCompilerOutputLine(this, new StringEventArgs("\r\n----------------------------------"));        
      }
      else if (e.ProgressPercentage == 3)
      {
        // option-svg ready
        string svgFile = (string)e.UserState;

        OptionFileReady(this, new StringEventArgs(svgFile));
      }
      else if (e.ProgressPercentage == 4)
      {
        // agent-svg ready
        string svgFile = (string)e.UserState;
        AgentFileReady(this, new StringEventArgs(svgFile));
      }
      else if (e.ProgressPercentage == -1)
      {
        // fatal error from XABSL-compiler
        ErrorMessageReceived(this, new StringEventArgs("fatal compile-error, please check compiler-messages"));
      }
      else if (e.ProgressPercentage == -2)
      {
        // general error
        ErrorMessageReceived(this, new StringEventArgs("an error occured during creation"));
      }
      else if (e.ProgressPercentage == -3)
      {
        ErrorMessageReceived(this, new StringEventArgs("The selected file has not been compiled. "
          + "Did you include it into \"agents" + XabslEnding + "\"?"));
      }
    }

    void backgroundWorkerUpdate_DoWork(object sender, System.ComponentModel.DoWorkEventArgs e)
    {
      // will create the images for state, option and agent
      try
      {
        if (!_isOption) return;

        backgroundWorkerUpdate.ReportProgress(0);
        if (backgroundWorkerUpdate.CancellationPending) return;

        // XABSL to xml //
        if (!xabsl2xml())
        {
          // an error occured during compiling, abort
          backgroundWorkerUpdate.ReportProgress(-1);
          return;
        }
                
        backgroundWorkerUpdate.ReportProgress(2);

        // generate the images //
        FileInfo xmlFile = new FileInfo(""
          + _behavior.BuildDir
          + "\\" + XabslBehavior.OPTIONS_DIR + "\\"
          + _curFile.Name.Remove(_curFile.Name.LastIndexOf(_curFile.Extension))
          + ".xml");

        if (!xmlFile.Exists)
        {
          backgroundWorkerUpdate.ReportProgress(-3);
          return;
        }

        XmlDocument doc = new XmlDocument();

        if (backgroundWorkerUpdate.CancellationPending) return;

        doc.Load(xmlFile.FullName);

        XmlNode opt = doc["option"];
        Directory.CreateDirectory(_resourceDir + "\\" + _behavior.BehaviorName);

        // option

        if (backgroundWorkerUpdate.CancellationPending) return;

        generateOptionGraph(opt,
            _resourceDir
            + _behavior.BehaviorName + "\\",
          _behavior.PathToDot);

        string svgFile1 =
            _resourceDir 
            + _behavior.BehaviorName + "\\option_"
            + opt.Attributes["name"].Value
            + ".svg";
        backgroundWorkerUpdate.ReportProgress(3, svgFile1);

        // agent-like look

        if (backgroundWorkerUpdate.CancellationPending) return;

        generateAgentGraph(opt,
            _resourceDir
            + _behavior.BehaviorName + "\\",
          _behavior.PathToDot);

        string svgFile2 =
            _resourceDir
            + _behavior.BehaviorName
            + "\\agent_"
            + opt.Attributes["name"].Value
            + ".svg";
        backgroundWorkerUpdate.ReportProgress(4, svgFile2);

      }
      catch (Exception exc)
      {
        backgroundWorkerUpdate.ReportProgress(-2);
        ExceptionDialog.ShowError(exc);
      }
    }

    /// <summary>
    /// Ask for creating the SVG-files for this document. Will fire the specific events when finished.
    /// </summary>
    /// <param name="forceUpdate">If false deliver already existing SVG-files. If true always regenerare these files</param>
    public void RequestSvgFiles (bool forceUpdate)
    {
      if (!_isOption)
      {
        return;
      }

      // update graphics if necessary
      try
      {
        // get the needed files for graphical output
        string agentFile;
        string optionFile;
        optionFile = agentFile = _resourceDir.FullName 
          + _behavior.BehaviorName
          + "\\";

        optionFile += "option_" + _optionName + ".svg";
        agentFile += "agent_" + _optionName + ".svg";

        // load graphics
        bool reload = false;
        if (!forceUpdate && System.IO.File.Exists(optionFile))
        {
          StringEventArgs args = new StringEventArgs(optionFile);
          OptionFileReady(this, args);
        }
        else
        {
          reload = true;
        }

        if (!forceUpdate && System.IO.File.Exists(agentFile))
        {
          StringEventArgs args = new StringEventArgs(agentFile);
          AgentFileReady(this, args);
        }
        else
        {
          reload = true;
        }

        if (reload || forceUpdate)
        {
          if (backgroundWorkerUpdate.IsBusy)
          {
            // cancel old one and restart another
            backgroundWorkerUpdate.CancelAsync();
            backgroundWorkerUpdate = new System.ComponentModel.BackgroundWorker();
            backgroundWorkerUpdate.DoWork += new System.ComponentModel.DoWorkEventHandler(backgroundWorkerUpdate_DoWork);
            backgroundWorkerUpdate.ProgressChanged += new System.ComponentModel.ProgressChangedEventHandler(backgroundWorkerUpdate_ProgressChanged);
            backgroundWorkerUpdate.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(backgroundWorkerUpdate_RunWorkerCompleted);
            backgroundWorkerUpdate.WorkerReportsProgress = true;
            backgroundWorkerUpdate.WorkerSupportsCancellation = true;

          }
          backgroundWorkerUpdate.RunWorkerAsync();
        }
        else
        {
          initAutoCompletion(_behavior.ApiFile);
        }
      }
      catch (Exception ex)
      {
        ExceptionDialog.ShowError(ex);
      }
      
    }

    /// <summary>
    /// Save changes made to the content of the document.
    /// </summary>
    /// <returns>Returns true if writing the file was successfull</returns>
    public bool SaveChanges()
    {
      try
      {
        StreamWriter writer = new StreamWriter(_curFile.FullName, false);
        writer.Write(_content);
        writer.Close();
        _curFile.Refresh();
        _timeOfLastSave = _curFile.LastWriteTime;
        _changesSaved = true;

        Saved(this, new EventArgs());

        return true;
      }
      catch (IOException ioE)
      {
        ExceptionDialog.ShowError(ioE);
        return false;
      }
    }

    /// <summary>
    /// Reloads the textual content from a file (e.g. when it had been changed outside)
    /// </summary>
    public void ReloadFileContent()
    {
      _curFile.Refresh();
      StreamReader reader = new StreamReader(_curFile.FullName);
      _content = reader.ReadToEnd();
      reader.Close();
      _changesSaved = true;
      _timeOfLastSave = _curFile.LastWriteTime;

    }

    /// <summary>
    /// Cancel all processes which are related to the compilation.
    /// </summary>
    public void CancelCompilation()
    {
      if (backgroundWorkerUpdate.IsBusy)
        backgroundWorkerUpdate.CancelAsync();
    }

    private void initAutoCompletion(string apiFile)
    {
      try
      {
        if (!System.IO.File.Exists(apiFile))
        {
          // something went wrong during compilation
          return;
        }

        StreamReader reader = new StreamReader(apiFile);

        int lines = 0;
        // first round, get number of lines
        while (!reader.EndOfStream)
        {
          reader.ReadLine();
          lines++;
        }
        reader.Close();

        // second round, get the lines
        _autoCList = "";
        string[] content = new string[lines];
        reader = new StreamReader(apiFile);
        for (int i = 0; i < lines && !reader.EndOfStream; i++)
        {
          content[i] = reader.ReadLine();
        }
        reader.Close();

        _autoCList = _autoCList.Replace("_", "");

        // sort
        Array.Sort<string>(content, new ScintillaCompare());

        // put together again
        for (int i = 0; i < lines; i++)
        {
          _autoCList = _autoCList + content[i] + "\n";
        }

      }
      catch (IOException ioE)
      {
        ExceptionDialog.ShowError(ioE);
      }
    }

    List<Transition> parseIfElseIfElse(XmlNode tree, string from)
    {
      XmlNode ifStatement;
      XmlNode elseIfStatement;
      XmlNode elseStatement;

      List<Transition> transitionList = new List<Transition>();

      transitionList.Capacity = 10;

      ifStatement = tree["if"];
      elseIfStatement = tree["else-if"];
      elseStatement = tree["else"];

      parseStatement(transitionList, ifStatement, from);
      parseStatement(transitionList, elseIfStatement, from);
      parseStatement(transitionList, elseStatement, from);

      // recursive
      if (ifStatement != null)
      {
        transitionList.AddRange(parseIfElseIfElse(ifStatement, from));
      }
      if (elseIfStatement != null)
      {
        transitionList.AddRange(parseIfElseIfElse(elseIfStatement, from));
      }
      if (elseStatement != null)
      {
        transitionList.AddRange(parseIfElseIfElse(elseStatement, from));
      }
      return transitionList;
    }

    void parseStatement(List<Transition> transitionList, XmlNode statement, string from)
    {
      if (statement != null)
      {
        // to other states
        XmlNode transitionState = statement["transition-to-state"];
        Transition trans = new Transition();

        if (transitionState != null)
        {
          trans.description = "";
          XmlNode condition = statement["condition"];
          if (condition != null)
          {
            string desc = "none";
            if (condition.Attributes["description"] != null)
            {
              desc = condition.Attributes["description"].Value;
            }
            trans.description = desc;
          }
          trans.to = transitionState.Attributes["ref"].Value;
          trans.from = from;
          transitionList.Add(trans);
        }
      }
    }

    /// <summary>
    /// Generates a graph that displays the option with it states.
    /// </summary>
    /// <param name="option"></param>
    /// <param name="pathToOutputDir"></param>
    /// <param name="pathToDotProgram"></param>
    private void generateOptionGraph(XmlNode option, string pathToOutputDir, string pathToDotProgram)
    {
      StreamWriter writer = new StreamWriter(pathToOutputDir + "option.dot");

      List<Transition> transitions = new List<Transition>();
      List<Transition> commonTrans = new List<Transition>();
      string optionName = option.Attributes["name"].Value;
      Hashtable underlying = new Hashtable();
      Hashtable alreadyWritten = new Hashtable();

      // head
      writer.Write("digraph option {\n");
      writer.Write("node [fontsize=\"10\"];\n");

      // get initial state
      string initialState = option.Attributes["initial-state"].Value;

      // common decision-tree
      XmlNode comTree = option["common-decision-tree"];
      if (comTree != null)
      {
        parseStatement(commonTrans, comTree,  "");
        commonTrans.AddRange(parseIfElseIfElse(comTree, ""));
      }

      // make a rectangle around the states of the option 
      writer.Write("subgraph cluster0 {\n");

      // go through each state
      foreach (XmlNode state in option)
      {
        if (state.Name.Equals("state"))
        {
          string name = state.Attributes["name"].Value;
          string orig_name = name;
          bool isInitial = name.Equals(initialState);
          name = name.Replace("_", "_\\n"); // just to fit nicer in the circle
          if (isInitial)
          {
            // intial
            writer.Write("\"" + name + "\" [shape=\"Mcircle\" ");
          }
          else if (state.Attributes["is-target-state"] != null && state.Attributes["is-target-state"].Value == "true")
          {
            // target
            writer.Write("\"" + name + "\" [shape=\"doublecircle\" ");
          }
          else
          {
            // normal
            writer.Write("\"" + name + "\" [shape=\"circle\" ");
          }

          writer.Write("URL=\"" + orig_name + ".statedummy\"];\n");

          // underlying options/basic-behaviors
          foreach (XmlNode n in state.ChildNodes)
          {
            if (n.Name == "subsequent-option")
            {
              if(!underlying.ContainsKey(state.Attributes["name"].Value))
              {
                underlying.Add(state.Attributes["name"].Value, new LinkedList<UnderlyingElement>());
              }
              LinkedList<UnderlyingElement> l = (LinkedList<UnderlyingElement>) underlying[state.Attributes["name"].Value];

              UnderlyingElement e = new UnderlyingElement();
              e.name = n.Attributes["ref"].Value;
              e.isOption = true;

              l.AddLast(e);
             
            }
            else if(n.Name == "subsequent-basic-behavior")
            {
              if(!underlying.ContainsKey(state.Attributes["name"].Value))
              {
                underlying.Add(state.Attributes["name"].Value, new LinkedList<UnderlyingElement>());
              }
              LinkedList<UnderlyingElement> l = (LinkedList<UnderlyingElement>) underlying[state.Attributes["name"].Value];

              UnderlyingElement e = new UnderlyingElement();
              e.name = n.Attributes["ref"].Value;
              e.isOption = false;

              l.AddLast(e);
            }
          }
          

          // get transitions
          XmlNode decTree = state["decision-tree"];
          if (decTree != null)
          {
            // maybe transitions without any condition
            parseStatement(transitions, decTree, name);

            // with condition
            transitions.AddRange(parseIfElseIfElse(decTree, name));
          }

          // write common transitions
          foreach (Transition t in commonTrans)
          {
            // don't write a transition twice
            string toWrite = "\"" + name + "\" -> \"" + t.to.Replace("_", "_\\n") + "\" [style=dotted]; \n";
            if (!alreadyWritten.ContainsKey(toWrite))
            {
              writer.Write(toWrite);
              alreadyWritten.Add(toWrite, true);
            }
          }
        }
      }

      writer.Write("}\n");

      // write out the transitions
      foreach (Transition t in transitions)
      {
        string toWrite = "\"" + t.from + "\" -> \"" + t.to.Replace("_", "_\\n") + "\"; \n";
        if (!alreadyWritten.ContainsKey(toWrite))
        {
          writer.Write(toWrite);
          alreadyWritten.Add(toWrite, true);
        }
      }

      // write out the underlying options
      foreach (string s in underlying.Keys)
      {
        LinkedList<UnderlyingElement> list = (LinkedList<UnderlyingElement>)underlying[s];
        foreach (UnderlyingElement e in list)
        {

          string to = "sub_" + e.name;
          to = to.Replace("_", "_\\n");
          string from = s.Replace("_", "_\\n");
          if (e.isOption)
          {
            writer.Write("\"" + to + "\" [label=\"" + e.name + "\" shape=\"plaintext\""
              + " URL=\"" + e.name + ".optdummy\"];\n");
          }
          else
          {
            writer.Write("\"" + to + "\" [label=\"" + e.name + "\" shape=\"box\"];\n");
          }
          writer.Write("\"" + from + "\" -> \"" + to + "\" [color=grey];\n");
        }

        //string to = "sub_" + underlying[s];
        //to = to.Replace("_", "_\\n");
        //string from = s.Replace("_", "_\\n");
        //if ((bool)isOption[underlying[s]])
        //{
        //  writer.Write("\"" + to + "\" [label=\"" + underlying[s] + "\" shape=\"plaintext\""
        //    + " URL=\"" + underlying[s] + ".optdummy\"];\n");
        //}
        //else
        //{
        //  writer.Write("\"" + to + "\" [label=\"" + underlying[s] + "\" shape=\"box\"];\n");
        //}
        //writer.Write("\"" + from + "\" -> \"" + to + "\" [color=grey];\n");
      }

      // finish
      writer.Write("}\n");
      writer.Close();

      // generate graphics using dot
      System.Diagnostics.Process p = new System.Diagnostics.Process();
      System.Diagnostics.ProcessStartInfo start = new System.Diagnostics.ProcessStartInfo(pathToDotProgram);

      start.UseShellExecute = false;
      start.Arguments = "option.dot -Tsvg -o option_" + optionName + ".svg";
      start.WorkingDirectory = pathToOutputDir;
      start.RedirectStandardError = true;
      start.CreateNoWindow = true;

      p.StartInfo = start;
      p.Start();

      lock (this)
      {
        while (!(p.HasExited))
        {
          if (backgroundWorkerUpdate.CancellationPending)
          {
            p.Kill();
          }
          // wait
          System.Threading.Monitor.Wait(this, 100);

        }
      }

      if (p.ExitCode != 0)
      {
        string error = "error when calling dot: " + p.StandardError.ReadToEnd();
        ExceptionDialog.ShowError(new Exception(error));
      }

    }

    /// <summary>
    /// Generate an agent-like graph starting at the given option.
    /// The old fashioned way
    /// </summary>
    /// <param name="node"></param>
    /// <param name="pathToOutputDir"></param>
    /// <param name="pathToDotProgram"></param>
    private void generateAgentGraph(XmlNode option, string pathToOutputDir, string pathToDotProgram)
    {
      string optionName = option.Attributes["name"].Value;

      // writer header of dot-file
      System.IO.StreamWriter writer = new System.IO.StreamWriter(pathToOutputDir
                                                                  + "agent_" + option.Attributes["name"].Value + ".dot", false);
      // header
      writer.Write("digraph option {\n");
      writer.Write("node [fontsize=\"10\"];\n");

      Hashtable alreadyWritten = new Hashtable();
      parseAgent(writer, option, pathToOutputDir, alreadyWritten);

      // footer
      writer.Write("}\n");
      writer.Close();

      // let dot create the graphics
      System.Diagnostics.Process p = new System.Diagnostics.Process();
      System.Diagnostics.ProcessStartInfo start = new System.Diagnostics.ProcessStartInfo(pathToDotProgram);

      start.UseShellExecute = false;
      start.Arguments = "agent_" + optionName + ".dot -Tsvg -o agent_" + optionName + ".svg";
      start.WorkingDirectory = pathToOutputDir;
      start.RedirectStandardError = true;
      start.CreateNoWindow = true;

      p.StartInfo = start;
      p.Start();

      lock (this)
      {
        while (!(p.HasExited))
        {
          if (backgroundWorkerUpdate.CancellationPending)
          {
            p.Kill();
          }
          // wait
          System.Threading.Monitor.Wait(this, 100);

        }
      }

      if (p.ExitCode != 0)
      {
        string error = "error when calling dot: " + p.StandardError.ReadToEnd();
        ExceptionDialog.ShowError(new Exception(error));
      }

    }

    private void parseAgent(StreamWriter writer, XmlNode option, string pathToOutputDir, Hashtable alreadyWritten)
    {
      List<Transition> transitions = new List<Transition>();
      List<UnderlyingElement> dependedOptions = new List<UnderlyingElement>();
      Hashtable isOption = new Hashtable();

      // the option itself
      string optionName = option.Attributes["name"].Value;
      writer.Write("\"" + optionName + "\" [URL=\"" + optionName + ".optdummy\"];\n");

      // common decision-tree
      XmlNode comTree = option["common-decision-tree"];
      if (comTree != null)
      {
        transitions.AddRange(parseIfElseIfElse(comTree, ""));
      }

      // get all states and it's underlying option/basic-behavior/symbols
      List<XmlNode> states = new List<XmlNode>();



      foreach (XmlNode n in option.ChildNodes)
      {
        if (n.Name == "state")
        {
          states.Add(n);
        }
      }

      Hashtable underlying = new Hashtable();

      foreach (XmlNode st in states)
      {
        foreach (XmlNode n in st.ChildNodes)
        {
          if (n.Name == "subsequent-option")
          {
            if (!underlying.ContainsKey(st.Attributes["name"].Value))
            {
              underlying.Add(st.Attributes["name"].Value, new LinkedList<UnderlyingElement>());
            }
            LinkedList<UnderlyingElement> l = (LinkedList<UnderlyingElement>)underlying[st.Attributes["name"].Value];

            UnderlyingElement e = new UnderlyingElement();
            e.name = n.Attributes["ref"].Value;
            e.isOption = true;

            l.AddLast(e);

          }
          else if (n.Name == "subsequent-basic-behavior")
          {
            if (!underlying.ContainsKey(st.Attributes["name"].Value))
            {
              underlying.Add(st.Attributes["name"].Value, new LinkedList<UnderlyingElement>());
            }
            LinkedList<UnderlyingElement> l = (LinkedList<UnderlyingElement>)underlying[st.Attributes["name"].Value];

            UnderlyingElement e = new UnderlyingElement();
            e.name = n.Attributes["ref"].Value;
            e.isOption = false;

            l.AddLast(e);
          }
        }
      }

      // search for transitions in all states
      foreach (XmlNode s in states)
      {
        XmlNode decTree = s["decision-tree"];
        parseStatement(transitions, decTree, "");
        transitions.AddRange(parseIfElseIfElse(decTree, ""));
      }

      // go through every transition and get the underlying option of the states
      foreach (Transition t in transitions)
      {

        if (underlying.ContainsKey(t.to))
        {
          string toWrite = "";
          LinkedList<UnderlyingElement> list = (LinkedList<UnderlyingElement>) underlying[t.to];

          foreach (UnderlyingElement e in list)
          {
            if (e.isOption)
            {
              toWrite = "\"" + optionName + "\" -> \"" +
                e.name + "\";\n";
              toWrite += "\"" + e.name + "\" [URL=\"" + e.name + ".optdummy\"];\n";
            }
            else
            {
              toWrite = "\"" + optionName + "\" -> \"" +
                e.name + "_basic\";\n";
            }
            // don't write a line twice
            if (!alreadyWritten.ContainsKey(toWrite))
            {
              writer.Write(toWrite);
              alreadyWritten.Add(toWrite, true);
              dependedOptions.Add(e);
            }
          }
        }
      }

      // include all sub-options
      foreach (UnderlyingElement e in dependedOptions)
      {
        FileInfo xmlFile = new FileInfo(""
          + _behavior.BuildDir
          + "\\" + XabslBehavior.OPTIONS_DIR + "\\"
          + e.name
          + ".xml");

        if (e.isOption && xmlFile.Exists)
        {
          try
          {
            XmlDocument doc = new XmlDocument();
            doc.Load(xmlFile.FullName);

            XmlNode opt = doc["option"];
            parseAgent(writer, opt, pathToOutputDir, alreadyWritten);
          }
          catch (Exception ex)
          {
            ExceptionDialog.ShowError(ex);
          }
        }
        else if(!e.isOption)
        {
          // basic behavior - is leaf
          writer.Write("\"" + e.name + "_basic\" [shape=box,label=\"" + e.name + "\"];\n");
        }
      }
    }


    private bool xabsl2xml()
    {

      if (!_isOption) return false;

      Process p = new Process();

      ProcessStartInfo start = new ProcessStartInfo();
      start.FileName = "ruby";

      string subdir = "";
      if (_curFile.Directory.Name.Equals("Options"))
      {
        subdir = "";
      }
      else
      {
        subdir = "\\" + _curFile.Directory.Name + "\\";
      }

      string buildDir = _behavior.BuildDir;

      Directory.CreateDirectory(buildDir);

      string args =
        "\"" + _behavior.CompilerPath
        + "\" -V "
        + "-I \"" + _behavior.DumpDir
        + "\" -a \"" + _behavior.ApiFile
        + "\" -x \"" + buildDir
        + "\" \"" + _behavior.Agent.FullName + "\"";
      start.Arguments = args.Replace('\\', '/');

      start.RedirectStandardOutput = true;
      start.RedirectStandardError = true;
      start.UseShellExecute = false;
      start.CreateNoWindow = true;

      p.StartInfo = start;
      p.ErrorDataReceived += new DataReceivedEventHandler(p_ErrorDataReceived);
      p.OutputDataReceived += new DataReceivedEventHandler(p_OutputDataReceived);
      p.EnableRaisingEvents = true;

      p.Start();
      p.BeginErrorReadLine();
      p.BeginOutputReadLine();

      lock (this)
      {
        while (!(p.HasExited))
        {
          if (backgroundWorkerUpdate.CancellationPending)
          {
            p.Kill();
          }
          // wait
          System.Threading.Monitor.Wait(this, 100);

        }
      }

      // check for fatal errors
      if (p.ExitCode != 0)
      {
        return false;
      }

      return true;
    }


    void p_OutputDataReceived(object sender, DataReceivedEventArgs e)
    {
      try
      {
        if (!backgroundWorkerUpdate.CancellationPending && backgroundWorkerUpdate.IsBusy)
        {
          backgroundWorkerUpdate.ReportProgress(1, e.Data);
        }
      }
      catch (Exception ex)
      {
        ExceptionDialog.ShowError(ex);
      }
    }

    void p_ErrorDataReceived(object sender, DataReceivedEventArgs e)
    {
      try
      {
        if (!backgroundWorkerUpdate.CancellationPending && backgroundWorkerUpdate.IsBusy)
        {
          backgroundWorkerUpdate.ReportProgress(1, e.Data);
        }

      }
      catch (Exception ex)
      {
        ExceptionDialog.ShowError(ex);
      }

    }

    /// <summary>
    /// Will look after the file names of the two XabslDocuments and wether they are equal.
    /// </summary>
    /// <param name="obj">The XabslDocument to compare with.</param>
    /// <returns>Wether the two XabslDocuments represent the same file.</returns>
    public override bool Equals(object obj)
    {
      try
      {
        if (obj != null)
        {
          XabslDocument doc = (XabslDocument)obj;
          if (doc.File.FullName.Equals(this.File.FullName, StringComparison.InvariantCultureIgnoreCase))
          {
            return true;
          }
        }
      }
      catch
      {
        return false;
      }

      return false;
    }

    /// <summary>
    /// Retreives a hash code for this object. Simply uses inherited function.
    /// </summary>
    /// <returns>Specific hash code for this instance.</returns>
    public override int GetHashCode()
    {
      return base.GetHashCode();
    }
  }

}
