namespace XabslEditor
{
  partial class MainForm
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
      this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
      this.statusStripMain = new System.Windows.Forms.StatusStrip();
      this.toolStripStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
      this.toolStripMain = new System.Windows.Forms.ToolStrip();
      this.buttonOpenBehavior = new System.Windows.Forms.ToolStripButton();
      this.buttonNewFile = new System.Windows.Forms.ToolStripButton();
      this.buttonOpen = new System.Windows.Forms.ToolStripButton();
      this.buttonSave = new System.Windows.Forms.ToolStripButton();
      this.buttonUpdate = new System.Windows.Forms.ToolStripButton();
      this.buttonUndo = new System.Windows.Forms.ToolStripButton();
      this.buttonRedo = new System.Windows.Forms.ToolStripButton();
      this.toolStripButtonFind = new System.Windows.Forms.ToolStripButton();
      this.buttonSearchInOptions = new System.Windows.Forms.ToolStripButton();
      this.buttonBack = new System.Windows.Forms.ToolStripButton();
      this.buttonForward = new System.Windows.Forms.ToolStripButton();
      this.dropDownButtonOption = new System.Windows.Forms.ToolStripDropDownButton();
      this.menuStripMain = new System.Windows.Forms.MenuStrip();
      this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.openBehaviorToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.recentBehaviorsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.recentOptionsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.newSingleFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.openSingleFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.saveCurrentFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
      this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.editToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.undoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.redoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
      this.findToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.searchInAllOptionsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.commentCodeOutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.uncommentCodeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.insertStateBodyToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.autocompletitionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
      this.configurationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.dummyOptionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.dummyTextToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.dummyViewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.dummyRecentOptionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.viewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.updateViewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
      this.newTabToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.closeTabToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripSeparator5 = new System.Windows.Forms.ToolStripSeparator();
      this.oneOptionBackToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.oneOptionForwardToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripSeparator6 = new System.Windows.Forms.ToolStripSeparator();
      this.clearCacheToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.infoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.splitContainerMain = new System.Windows.Forms.SplitContainer();
      this.tabControlEditor = new System.Windows.Forms.TabControl();
      this.tabControlMain = new System.Windows.Forms.TabControl();
      this.tabPageOption = new System.Windows.Forms.TabPage();
      this.labelEnableGraphView = new System.Windows.Forms.Label();
      this.webBrowserOption = new System.Windows.Forms.WebBrowser();
      this.tabPageAgent = new System.Windows.Forms.TabPage();
      this.webBrowserAgent = new System.Windows.Forms.WebBrowser();
      this.tabPageErrors = new System.Windows.Forms.TabPage();
      this.treeViewErrors = new System.Windows.Forms.TreeView();
      this.imageListMain = new System.Windows.Forms.ImageList(this.components);
      this.tabPageCompiler = new System.Windows.Forms.TabPage();
      this.textBoxCompiler = new System.Windows.Forms.TextBox();
      this.folderBrowserDialog1 = new System.Windows.Forms.FolderBrowserDialog();
      this.saveFileDialog = new System.Windows.Forms.SaveFileDialog();
      this.timerCheckForChangedFile = new System.Windows.Forms.Timer(this.components);
      this.dropDownButtonAgent = new System.Windows.Forms.ToolStripDropDownButton();
      this.statusStripMain.SuspendLayout();
      this.toolStripMain.SuspendLayout();
      this.menuStripMain.SuspendLayout();
      this.splitContainerMain.Panel1.SuspendLayout();
      this.splitContainerMain.Panel2.SuspendLayout();
      this.splitContainerMain.SuspendLayout();
      this.tabControlMain.SuspendLayout();
      this.tabPageOption.SuspendLayout();
      this.tabPageAgent.SuspendLayout();
      this.tabPageErrors.SuspendLayout();
      this.tabPageCompiler.SuspendLayout();
      this.SuspendLayout();
      // 
      // openFileDialog
      // 
      this.openFileDialog.DefaultExt = "xabsl";
      this.openFileDialog.Filter = "XABSL Files (*.xabsl) | *.xabsl|All Files (*.*) | *.*";
      this.openFileDialog.Title = "Open file";
      // 
      // statusStripMain
      // 
      this.statusStripMain.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel});
      this.statusStripMain.Location = new System.Drawing.Point(0, 466);
      this.statusStripMain.Name = "statusStripMain";
      this.statusStripMain.Size = new System.Drawing.Size(720, 22);
      this.statusStripMain.TabIndex = 1;
      // 
      // toolStripStatusLabel
      // 
      this.toolStripStatusLabel.Name = "toolStripStatusLabel";
      this.toolStripStatusLabel.Size = new System.Drawing.Size(705, 17);
      this.toolStripStatusLabel.Spring = true;
      this.toolStripStatusLabel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
      // 
      // toolStripMain
      // 
      this.toolStripMain.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.buttonOpenBehavior,
            this.buttonNewFile,
            this.buttonOpen,
            this.buttonSave,
            this.buttonUpdate,
            this.buttonUndo,
            this.buttonRedo,
            this.toolStripButtonFind,
            this.buttonSearchInOptions,
            this.buttonBack,
            this.buttonForward,
            this.dropDownButtonOption,
            this.dropDownButtonAgent});
      this.toolStripMain.Location = new System.Drawing.Point(0, 24);
      this.toolStripMain.Name = "toolStripMain";
      this.toolStripMain.Size = new System.Drawing.Size(720, 25);
      this.toolStripMain.TabIndex = 0;
      this.toolStripMain.Text = "toolStripMain";
      // 
      // buttonOpenBehavior
      // 
      this.buttonOpenBehavior.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.buttonOpenBehavior.Image = global::XabslEditor.Properties.Resources.Book_openHS;
      this.buttonOpenBehavior.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.buttonOpenBehavior.Name = "buttonOpenBehavior";
      this.buttonOpenBehavior.Size = new System.Drawing.Size(23, 22);
      this.buttonOpenBehavior.Text = "Open Behavior";
      this.buttonOpenBehavior.Click += new System.EventHandler(this.openBehaviorToolStripMenuItem_Click);
      // 
      // buttonNewFile
      // 
      this.buttonNewFile.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.buttonNewFile.Image = global::XabslEditor.Properties.Resources.NewDocumentHS;
      this.buttonNewFile.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.buttonNewFile.Name = "buttonNewFile";
      this.buttonNewFile.Size = new System.Drawing.Size(23, 22);
      this.buttonNewFile.Text = "New Single File";
      this.buttonNewFile.Click += new System.EventHandler(this.buttonNewFile_Click);
      // 
      // buttonOpen
      // 
      this.buttonOpen.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.buttonOpen.Image = global::XabslEditor.Properties.Resources.openHS;
      this.buttonOpen.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.buttonOpen.Name = "buttonOpen";
      this.buttonOpen.Size = new System.Drawing.Size(23, 22);
      this.buttonOpen.Text = "Open Single File";
      this.buttonOpen.Click += new System.EventHandler(this.buttonOpen_Click);
      // 
      // buttonSave
      // 
      this.buttonSave.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.buttonSave.Enabled = false;
      this.buttonSave.Image = global::XabslEditor.Properties.Resources.saveHS;
      this.buttonSave.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.buttonSave.Name = "buttonSave";
      this.buttonSave.Size = new System.Drawing.Size(23, 22);
      this.buttonSave.Text = "Save";
      this.buttonSave.Click += new System.EventHandler(this.buttonSave_Click);
      // 
      // buttonUpdate
      // 
      this.buttonUpdate.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.buttonUpdate.Image = global::XabslEditor.Properties.Resources.RefreshDocViewHS;
      this.buttonUpdate.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.buttonUpdate.Name = "buttonUpdate";
      this.buttonUpdate.Size = new System.Drawing.Size(23, 22);
      this.buttonUpdate.Text = "Update View";
      this.buttonUpdate.Click += new System.EventHandler(this.updateViewToolStripMenuItem_Click);
      // 
      // buttonUndo
      // 
      this.buttonUndo.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.buttonUndo.Image = global::XabslEditor.Properties.Resources.Edit_UndoHS;
      this.buttonUndo.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.buttonUndo.Name = "buttonUndo";
      this.buttonUndo.Size = new System.Drawing.Size(23, 22);
      this.buttonUndo.Text = "Undo";
      this.buttonUndo.Click += new System.EventHandler(this.buttonUndo_Click);
      // 
      // buttonRedo
      // 
      this.buttonRedo.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.buttonRedo.Image = global::XabslEditor.Properties.Resources.Edit_RedoHS;
      this.buttonRedo.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.buttonRedo.Name = "buttonRedo";
      this.buttonRedo.Size = new System.Drawing.Size(23, 22);
      this.buttonRedo.Text = "Redo";
      this.buttonRedo.Click += new System.EventHandler(this.buttonRedo_Click);
      // 
      // toolStripButtonFind
      // 
      this.toolStripButtonFind.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButtonFind.Image = global::XabslEditor.Properties.Resources.FindHS;
      this.toolStripButtonFind.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButtonFind.Name = "toolStripButtonFind";
      this.toolStripButtonFind.Size = new System.Drawing.Size(23, 22);
      this.toolStripButtonFind.Text = "Find";
      this.toolStripButtonFind.Click += new System.EventHandler(this.findToolStripMenuItem_Click);
      // 
      // buttonSearchInOptions
      // 
      this.buttonSearchInOptions.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.buttonSearchInOptions.Enabled = false;
      this.buttonSearchInOptions.Image = global::XabslEditor.Properties.Resources.SearchFolderHS;
      this.buttonSearchInOptions.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.buttonSearchInOptions.Name = "buttonSearchInOptions";
      this.buttonSearchInOptions.Size = new System.Drawing.Size(23, 22);
      this.buttonSearchInOptions.Text = "Search in all options";
      this.buttonSearchInOptions.Click += new System.EventHandler(this.toolStripButtonSearchInOptions_Click);
      // 
      // buttonBack
      // 
      this.buttonBack.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.buttonBack.Enabled = false;
      this.buttonBack.Image = global::XabslEditor.Properties.Resources.NavBack;
      this.buttonBack.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.buttonBack.Name = "buttonBack";
      this.buttonBack.Size = new System.Drawing.Size(23, 22);
      this.buttonBack.Text = "Back";
      this.buttonBack.Click += new System.EventHandler(this.buttonBack_Click);
      // 
      // buttonForward
      // 
      this.buttonForward.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.buttonForward.Enabled = false;
      this.buttonForward.Image = global::XabslEditor.Properties.Resources.NavForward;
      this.buttonForward.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.buttonForward.Name = "buttonForward";
      this.buttonForward.Size = new System.Drawing.Size(23, 22);
      this.buttonForward.Text = "Forward";
      this.buttonForward.Click += new System.EventHandler(this.buttonForward_Click);
      // 
      // dropDownButtonOption
      // 
      this.dropDownButtonOption.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
      this.dropDownButtonOption.Image = ((System.Drawing.Image)(resources.GetObject("dropDownButtonOption.Image")));
      this.dropDownButtonOption.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.dropDownButtonOption.Name = "dropDownButtonOption";
      this.dropDownButtonOption.Size = new System.Drawing.Size(110, 22);
      this.dropDownButtonOption.Text = "(choose an option)";
      // 
      // menuStripMain
      // 
      this.menuStripMain.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.editToolStripMenuItem,
            this.viewToolStripMenuItem,
            this.helpToolStripMenuItem});
      this.menuStripMain.Location = new System.Drawing.Point(0, 0);
      this.menuStripMain.Name = "menuStripMain";
      this.menuStripMain.Size = new System.Drawing.Size(720, 24);
      this.menuStripMain.TabIndex = 5;
      this.menuStripMain.Text = "menuStrip1";
      // 
      // fileToolStripMenuItem
      // 
      this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openBehaviorToolStripMenuItem,
            this.recentBehaviorsToolStripMenuItem,
            this.recentOptionsToolStripMenuItem,
            this.newSingleFileToolStripMenuItem,
            this.openSingleFileToolStripMenuItem,
            this.saveCurrentFileToolStripMenuItem,
            this.toolStripSeparator1,
            this.exitToolStripMenuItem});
      this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
      this.fileToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
      this.fileToolStripMenuItem.Text = "&File";
      // 
      // openBehaviorToolStripMenuItem
      // 
      this.openBehaviorToolStripMenuItem.Name = "openBehaviorToolStripMenuItem";
      this.openBehaviorToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.B)));
      this.openBehaviorToolStripMenuItem.Size = new System.Drawing.Size(206, 22);
      this.openBehaviorToolStripMenuItem.Text = "Open &Behavior";
      this.openBehaviorToolStripMenuItem.Click += new System.EventHandler(this.openBehaviorToolStripMenuItem_Click);
      // 
      // recentBehaviorsToolStripMenuItem
      // 
      this.recentBehaviorsToolStripMenuItem.Enabled = false;
      this.recentBehaviorsToolStripMenuItem.Name = "recentBehaviorsToolStripMenuItem";
      this.recentBehaviorsToolStripMenuItem.Size = new System.Drawing.Size(206, 22);
      this.recentBehaviorsToolStripMenuItem.Text = "Recent &Behaviors";
      // 
      // recentOptionsToolStripMenuItem
      // 
      this.recentOptionsToolStripMenuItem.Enabled = false;
      this.recentOptionsToolStripMenuItem.Name = "recentOptionsToolStripMenuItem";
      this.recentOptionsToolStripMenuItem.Size = new System.Drawing.Size(206, 22);
      this.recentOptionsToolStripMenuItem.Text = "&Recent Options";
      // 
      // newSingleFileToolStripMenuItem
      // 
      this.newSingleFileToolStripMenuItem.Name = "newSingleFileToolStripMenuItem";
      this.newSingleFileToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.N)));
      this.newSingleFileToolStripMenuItem.Size = new System.Drawing.Size(206, 22);
      this.newSingleFileToolStripMenuItem.Text = "&New Single File";
      this.newSingleFileToolStripMenuItem.Click += new System.EventHandler(this.buttonNewFile_Click);
      // 
      // openSingleFileToolStripMenuItem
      // 
      this.openSingleFileToolStripMenuItem.Name = "openSingleFileToolStripMenuItem";
      this.openSingleFileToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
      this.openSingleFileToolStripMenuItem.Size = new System.Drawing.Size(206, 22);
      this.openSingleFileToolStripMenuItem.Text = "&Open Single File";
      this.openSingleFileToolStripMenuItem.Click += new System.EventHandler(this.buttonOpen_Click);
      // 
      // saveCurrentFileToolStripMenuItem
      // 
      this.saveCurrentFileToolStripMenuItem.Name = "saveCurrentFileToolStripMenuItem";
      this.saveCurrentFileToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
      this.saveCurrentFileToolStripMenuItem.Size = new System.Drawing.Size(206, 22);
      this.saveCurrentFileToolStripMenuItem.Text = "&Save Current File";
      this.saveCurrentFileToolStripMenuItem.Click += new System.EventHandler(this.buttonSave_Click);
      // 
      // toolStripSeparator1
      // 
      this.toolStripSeparator1.Name = "toolStripSeparator1";
      this.toolStripSeparator1.Size = new System.Drawing.Size(203, 6);
      // 
      // exitToolStripMenuItem
      // 
      this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
      this.exitToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Q)));
      this.exitToolStripMenuItem.Size = new System.Drawing.Size(206, 22);
      this.exitToolStripMenuItem.Text = "&Exit";
      this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
      // 
      // editToolStripMenuItem
      // 
      this.editToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.undoToolStripMenuItem,
            this.redoToolStripMenuItem,
            this.toolStripSeparator3,
            this.findToolStripMenuItem,
            this.searchInAllOptionsToolStripMenuItem,
            this.commentCodeOutToolStripMenuItem,
            this.uncommentCodeToolStripMenuItem,
            this.insertStateBodyToolStripMenuItem,
            this.autocompletitionToolStripMenuItem,
            this.toolStripSeparator4,
            this.configurationToolStripMenuItem,
            this.dummyOptionToolStripMenuItem,
            this.dummyTextToolStripMenuItem,
            this.dummyViewToolStripMenuItem,
            this.dummyRecentOptionToolStripMenuItem});
      this.editToolStripMenuItem.Name = "editToolStripMenuItem";
      this.editToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
      this.editToolStripMenuItem.Text = "&Edit";
      // 
      // undoToolStripMenuItem
      // 
      this.undoToolStripMenuItem.Name = "undoToolStripMenuItem";
      this.undoToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Z)));
      this.undoToolStripMenuItem.Size = new System.Drawing.Size(253, 22);
      this.undoToolStripMenuItem.Text = "U&ndo";
      this.undoToolStripMenuItem.Click += new System.EventHandler(this.buttonUndo_Click);
      // 
      // redoToolStripMenuItem
      // 
      this.redoToolStripMenuItem.Name = "redoToolStripMenuItem";
      this.redoToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Y)));
      this.redoToolStripMenuItem.Size = new System.Drawing.Size(253, 22);
      this.redoToolStripMenuItem.Text = "&Redo";
      this.redoToolStripMenuItem.Click += new System.EventHandler(this.buttonRedo_Click);
      // 
      // toolStripSeparator3
      // 
      this.toolStripSeparator3.Name = "toolStripSeparator3";
      this.toolStripSeparator3.Size = new System.Drawing.Size(250, 6);
      // 
      // findToolStripMenuItem
      // 
      this.findToolStripMenuItem.Name = "findToolStripMenuItem";
      this.findToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.F)));
      this.findToolStripMenuItem.Size = new System.Drawing.Size(253, 22);
      this.findToolStripMenuItem.Text = "&Find";
      this.findToolStripMenuItem.Click += new System.EventHandler(this.findToolStripMenuItem_Click);
      // 
      // searchInAllOptionsToolStripMenuItem
      // 
      this.searchInAllOptionsToolStripMenuItem.Enabled = false;
      this.searchInAllOptionsToolStripMenuItem.Name = "searchInAllOptionsToolStripMenuItem";
      this.searchInAllOptionsToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift)
                  | System.Windows.Forms.Keys.F)));
      this.searchInAllOptionsToolStripMenuItem.Size = new System.Drawing.Size(253, 22);
      this.searchInAllOptionsToolStripMenuItem.Text = "&Search In All Options";
      this.searchInAllOptionsToolStripMenuItem.Click += new System.EventHandler(this.toolStripButtonSearchInOptions_Click);
      // 
      // commentCodeOutToolStripMenuItem
      // 
      this.commentCodeOutToolStripMenuItem.Name = "commentCodeOutToolStripMenuItem";
      this.commentCodeOutToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.D7)));
      this.commentCodeOutToolStripMenuItem.Size = new System.Drawing.Size(253, 22);
      this.commentCodeOutToolStripMenuItem.Text = "&Comment Code Out";
      this.commentCodeOutToolStripMenuItem.Click += new System.EventHandler(this.commentCodeOutToolStripMenuItem_Click);
      // 
      // uncommentCodeToolStripMenuItem
      // 
      this.uncommentCodeToolStripMenuItem.Name = "uncommentCodeToolStripMenuItem";
      this.uncommentCodeToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.D8)));
      this.uncommentCodeToolStripMenuItem.Size = new System.Drawing.Size(253, 22);
      this.uncommentCodeToolStripMenuItem.Text = "&Uncomment Code";
      this.uncommentCodeToolStripMenuItem.Click += new System.EventHandler(this.uncommentCodeToolStripMenuItem_Click);
      // 
      // insertStateBodyToolStripMenuItem
      // 
      this.insertStateBodyToolStripMenuItem.Name = "insertStateBodyToolStripMenuItem";
      this.insertStateBodyToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.I)));
      this.insertStateBodyToolStripMenuItem.Size = new System.Drawing.Size(253, 22);
      this.insertStateBodyToolStripMenuItem.Text = "Insert Standard State &Body";
      this.insertStateBodyToolStripMenuItem.Click += new System.EventHandler(this.insertStateBodyToolStripMenuItem_Click);
      // 
      // autocompletitionToolStripMenuItem
      // 
      this.autocompletitionToolStripMenuItem.Name = "autocompletitionToolStripMenuItem";
      this.autocompletitionToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Space)));
      this.autocompletitionToolStripMenuItem.Size = new System.Drawing.Size(253, 22);
      this.autocompletitionToolStripMenuItem.Text = "&Autocompletition";
      this.autocompletitionToolStripMenuItem.Click += new System.EventHandler(this.autocompletitionToolStripMenuItem_Click);
      // 
      // toolStripSeparator4
      // 
      this.toolStripSeparator4.Name = "toolStripSeparator4";
      this.toolStripSeparator4.Size = new System.Drawing.Size(250, 6);
      // 
      // configurationToolStripMenuItem
      // 
      this.configurationToolStripMenuItem.Enabled = false;
      this.configurationToolStripMenuItem.Name = "configurationToolStripMenuItem";
      this.configurationToolStripMenuItem.Size = new System.Drawing.Size(253, 22);
      this.configurationToolStripMenuItem.Text = "&Configuration";
      this.configurationToolStripMenuItem.Click += new System.EventHandler(this.configurationToolStripMenuItem_Click);
      // 
      // dummyOptionToolStripMenuItem
      // 
      this.dummyOptionToolStripMenuItem.Name = "dummyOptionToolStripMenuItem";
      this.dummyOptionToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F6;
      this.dummyOptionToolStripMenuItem.Size = new System.Drawing.Size(253, 22);
      this.dummyOptionToolStripMenuItem.Text = "dummyOption";
      this.dummyOptionToolStripMenuItem.Visible = false;
      this.dummyOptionToolStripMenuItem.Click += new System.EventHandler(this.dummyOptionToolStripMenuItem_Click);
      // 
      // dummyTextToolStripMenuItem
      // 
      this.dummyTextToolStripMenuItem.Name = "dummyTextToolStripMenuItem";
      this.dummyTextToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F7;
      this.dummyTextToolStripMenuItem.Size = new System.Drawing.Size(253, 22);
      this.dummyTextToolStripMenuItem.Text = "dummyText";
      this.dummyTextToolStripMenuItem.Visible = false;
      this.dummyTextToolStripMenuItem.Click += new System.EventHandler(this.dummyTextToolStripMenuItem_Click);
      // 
      // dummyViewToolStripMenuItem
      // 
      this.dummyViewToolStripMenuItem.Name = "dummyViewToolStripMenuItem";
      this.dummyViewToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F8;
      this.dummyViewToolStripMenuItem.Size = new System.Drawing.Size(253, 22);
      this.dummyViewToolStripMenuItem.Text = "dummyView";
      this.dummyViewToolStripMenuItem.Visible = false;
      this.dummyViewToolStripMenuItem.Click += new System.EventHandler(this.dummyViewToolStripMenuItem_Click);
      // 
      // dummyRecentOptionToolStripMenuItem
      // 
      this.dummyRecentOptionToolStripMenuItem.Name = "dummyRecentOptionToolStripMenuItem";
      this.dummyRecentOptionToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F9;
      this.dummyRecentOptionToolStripMenuItem.Size = new System.Drawing.Size(253, 22);
      this.dummyRecentOptionToolStripMenuItem.Text = "dummyRecentOption";
      this.dummyRecentOptionToolStripMenuItem.Visible = false;
      this.dummyRecentOptionToolStripMenuItem.Click += new System.EventHandler(this.dummyRecentOptionToolStripMenuItem_Click);
      // 
      // viewToolStripMenuItem
      // 
      this.viewToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.updateViewToolStripMenuItem,
            this.toolStripSeparator2,
            this.newTabToolStripMenuItem,
            this.closeTabToolStripMenuItem,
            this.toolStripSeparator5,
            this.oneOptionBackToolStripMenuItem,
            this.oneOptionForwardToolStripMenuItem,
            this.toolStripSeparator6,
            this.clearCacheToolStripMenuItem});
      this.viewToolStripMenuItem.Name = "viewToolStripMenuItem";
      this.viewToolStripMenuItem.Size = new System.Drawing.Size(41, 20);
      this.viewToolStripMenuItem.Text = "&View";
      // 
      // updateViewToolStripMenuItem
      // 
      this.updateViewToolStripMenuItem.Name = "updateViewToolStripMenuItem";
      this.updateViewToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F5;
      this.updateViewToolStripMenuItem.Size = new System.Drawing.Size(183, 22);
      this.updateViewToolStripMenuItem.Text = "&Update View";
      this.updateViewToolStripMenuItem.Click += new System.EventHandler(this.updateViewToolStripMenuItem_Click);
      // 
      // toolStripSeparator2
      // 
      this.toolStripSeparator2.Name = "toolStripSeparator2";
      this.toolStripSeparator2.Size = new System.Drawing.Size(180, 6);
      // 
      // newTabToolStripMenuItem
      // 
      this.newTabToolStripMenuItem.Name = "newTabToolStripMenuItem";
      this.newTabToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.T)));
      this.newTabToolStripMenuItem.Size = new System.Drawing.Size(183, 22);
      this.newTabToolStripMenuItem.Text = "&New Tab";
      this.newTabToolStripMenuItem.Click += new System.EventHandler(this.newTabToolStripMenuItem_Click);
      // 
      // closeTabToolStripMenuItem
      // 
      this.closeTabToolStripMenuItem.Name = "closeTabToolStripMenuItem";
      this.closeTabToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.W)));
      this.closeTabToolStripMenuItem.Size = new System.Drawing.Size(183, 22);
      this.closeTabToolStripMenuItem.Text = "Cl&ose Tab";
      this.closeTabToolStripMenuItem.Click += new System.EventHandler(this.closeTabToolStripMenuItem_Click);
      // 
      // toolStripSeparator5
      // 
      this.toolStripSeparator5.Name = "toolStripSeparator5";
      this.toolStripSeparator5.Size = new System.Drawing.Size(180, 6);
      // 
      // oneOptionBackToolStripMenuItem
      // 
      this.oneOptionBackToolStripMenuItem.Name = "oneOptionBackToolStripMenuItem";
      this.oneOptionBackToolStripMenuItem.Size = new System.Drawing.Size(183, 22);
      this.oneOptionBackToolStripMenuItem.Text = "One Option &Back";
      this.oneOptionBackToolStripMenuItem.Click += new System.EventHandler(this.buttonBack_Click);
      // 
      // oneOptionForwardToolStripMenuItem
      // 
      this.oneOptionForwardToolStripMenuItem.Name = "oneOptionForwardToolStripMenuItem";
      this.oneOptionForwardToolStripMenuItem.Size = new System.Drawing.Size(183, 22);
      this.oneOptionForwardToolStripMenuItem.Text = "One Option &Forward";
      this.oneOptionForwardToolStripMenuItem.Click += new System.EventHandler(this.buttonForward_Click);
      // 
      // toolStripSeparator6
      // 
      this.toolStripSeparator6.Name = "toolStripSeparator6";
      this.toolStripSeparator6.Size = new System.Drawing.Size(180, 6);
      // 
      // clearCacheToolStripMenuItem
      // 
      this.clearCacheToolStripMenuItem.Enabled = false;
      this.clearCacheToolStripMenuItem.Name = "clearCacheToolStripMenuItem";
      this.clearCacheToolStripMenuItem.Size = new System.Drawing.Size(183, 22);
      this.clearCacheToolStripMenuItem.Text = "&Clear Cache";
      this.clearCacheToolStripMenuItem.Click += new System.EventHandler(this.clearCacheToolStripMenuItem_Click);
      // 
      // helpToolStripMenuItem
      // 
      this.helpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.infoToolStripMenuItem});
      this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
      this.helpToolStripMenuItem.Size = new System.Drawing.Size(40, 20);
      this.helpToolStripMenuItem.Text = "&Help";
      // 
      // infoToolStripMenuItem
      // 
      this.infoToolStripMenuItem.Name = "infoToolStripMenuItem";
      this.infoToolStripMenuItem.Size = new System.Drawing.Size(114, 22);
      this.infoToolStripMenuItem.Text = "&About";
      this.infoToolStripMenuItem.Click += new System.EventHandler(this.infoToolStripMenuItem_Click);
      // 
      // splitContainerMain
      // 
      this.splitContainerMain.Dock = System.Windows.Forms.DockStyle.Fill;
      this.splitContainerMain.Location = new System.Drawing.Point(0, 49);
      this.splitContainerMain.Name = "splitContainerMain";
      // 
      // splitContainerMain.Panel1
      // 
      this.splitContainerMain.Panel1.Controls.Add(this.tabControlEditor);
      // 
      // splitContainerMain.Panel2
      // 
      this.splitContainerMain.Panel2.Controls.Add(this.tabControlMain);
      this.splitContainerMain.Size = new System.Drawing.Size(720, 417);
      this.splitContainerMain.SplitterDistance = 372;
      this.splitContainerMain.TabIndex = 6;
      // 
      // tabControlEditor
      // 
      this.tabControlEditor.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tabControlEditor.Location = new System.Drawing.Point(0, 0);
      this.tabControlEditor.Name = "tabControlEditor";
      this.tabControlEditor.SelectedIndex = 0;
      this.tabControlEditor.Size = new System.Drawing.Size(372, 417);
      this.tabControlEditor.TabIndex = 0;
      this.tabControlEditor.SelectedIndexChanged += new System.EventHandler(this.tabControlEditor_SelectedIndexChanged);
      // 
      // tabControlMain
      // 
      this.tabControlMain.Controls.Add(this.tabPageOption);
      this.tabControlMain.Controls.Add(this.tabPageAgent);
      this.tabControlMain.Controls.Add(this.tabPageErrors);
      this.tabControlMain.Controls.Add(this.tabPageCompiler);
      this.tabControlMain.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tabControlMain.ImageList = this.imageListMain;
      this.tabControlMain.Location = new System.Drawing.Point(0, 0);
      this.tabControlMain.Name = "tabControlMain";
      this.tabControlMain.SelectedIndex = 0;
      this.tabControlMain.Size = new System.Drawing.Size(344, 417);
      this.tabControlMain.TabIndex = 2;
      // 
      // tabPageOption
      // 
      this.tabPageOption.Controls.Add(this.labelEnableGraphView);
      this.tabPageOption.Controls.Add(this.webBrowserOption);
      this.tabPageOption.Location = new System.Drawing.Point(4, 23);
      this.tabPageOption.Name = "tabPageOption";
      this.tabPageOption.Padding = new System.Windows.Forms.Padding(3);
      this.tabPageOption.Size = new System.Drawing.Size(336, 390);
      this.tabPageOption.TabIndex = 0;
      this.tabPageOption.Text = "option";
      this.tabPageOption.UseVisualStyleBackColor = true;
      // 
      // labelEnableGraphView
      // 
      this.labelEnableGraphView.Dock = System.Windows.Forms.DockStyle.Fill;
      this.labelEnableGraphView.Location = new System.Drawing.Point(3, 3);
      this.labelEnableGraphView.Name = "labelEnableGraphView";
      this.labelEnableGraphView.Size = new System.Drawing.Size(330, 384);
      this.labelEnableGraphView.TabIndex = 7;
      this.labelEnableGraphView.Text = "no graphical view available for this file";
      this.labelEnableGraphView.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
      this.labelEnableGraphView.Visible = false;
      // 
      // webBrowserOption
      // 
      this.webBrowserOption.Dock = System.Windows.Forms.DockStyle.Fill;
      this.webBrowserOption.Location = new System.Drawing.Point(3, 3);
      this.webBrowserOption.MinimumSize = new System.Drawing.Size(20, 20);
      this.webBrowserOption.Name = "webBrowserOption";
      this.webBrowserOption.Size = new System.Drawing.Size(330, 384);
      this.webBrowserOption.TabIndex = 0;
      this.webBrowserOption.WebBrowserShortcutsEnabled = false;
      this.webBrowserOption.Navigating += new System.Windows.Forms.WebBrowserNavigatingEventHandler(this.webBrowserOption_Navigating);
      // 
      // tabPageAgent
      // 
      this.tabPageAgent.Controls.Add(this.webBrowserAgent);
      this.tabPageAgent.Location = new System.Drawing.Point(4, 23);
      this.tabPageAgent.Name = "tabPageAgent";
      this.tabPageAgent.Padding = new System.Windows.Forms.Padding(3);
      this.tabPageAgent.Size = new System.Drawing.Size(336, 390);
      this.tabPageAgent.TabIndex = 1;
      this.tabPageAgent.Text = "agent";
      this.tabPageAgent.UseVisualStyleBackColor = true;
      // 
      // webBrowserAgent
      // 
      this.webBrowserAgent.Dock = System.Windows.Forms.DockStyle.Fill;
      this.webBrowserAgent.Location = new System.Drawing.Point(3, 3);
      this.webBrowserAgent.MinimumSize = new System.Drawing.Size(20, 20);
      this.webBrowserAgent.Name = "webBrowserAgent";
      this.webBrowserAgent.Size = new System.Drawing.Size(330, 384);
      this.webBrowserAgent.TabIndex = 0;
      this.webBrowserAgent.WebBrowserShortcutsEnabled = false;
      this.webBrowserAgent.Navigating += new System.Windows.Forms.WebBrowserNavigatingEventHandler(this.webBrowserOption_Navigating);
      // 
      // tabPageErrors
      // 
      this.tabPageErrors.Controls.Add(this.treeViewErrors);
      this.tabPageErrors.Location = new System.Drawing.Point(4, 23);
      this.tabPageErrors.Name = "tabPageErrors";
      this.tabPageErrors.Padding = new System.Windows.Forms.Padding(3);
      this.tabPageErrors.Size = new System.Drawing.Size(336, 390);
      this.tabPageErrors.TabIndex = 3;
      this.tabPageErrors.Text = "errors";
      this.tabPageErrors.UseVisualStyleBackColor = true;
      // 
      // treeViewErrors
      // 
      this.treeViewErrors.Dock = System.Windows.Forms.DockStyle.Fill;
      this.treeViewErrors.ImageIndex = 0;
      this.treeViewErrors.ImageList = this.imageListMain;
      this.treeViewErrors.Location = new System.Drawing.Point(3, 3);
      this.treeViewErrors.Name = "treeViewErrors";
      this.treeViewErrors.SelectedImageIndex = 0;
      this.treeViewErrors.Size = new System.Drawing.Size(330, 384);
      this.treeViewErrors.TabIndex = 0;
      // 
      // imageListMain
      // 
      this.imageListMain.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageListMain.ImageStream")));
      this.imageListMain.TransparentColor = System.Drawing.Color.Fuchsia;
      this.imageListMain.Images.SetKeyName(0, "Error.bmp");
      this.imageListMain.Images.SetKeyName(1, "OK.bmp");
      // 
      // tabPageCompiler
      // 
      this.tabPageCompiler.Controls.Add(this.textBoxCompiler);
      this.tabPageCompiler.Location = new System.Drawing.Point(4, 23);
      this.tabPageCompiler.Name = "tabPageCompiler";
      this.tabPageCompiler.Padding = new System.Windows.Forms.Padding(3);
      this.tabPageCompiler.Size = new System.Drawing.Size(336, 390);
      this.tabPageCompiler.TabIndex = 2;
      this.tabPageCompiler.Text = "compiler output";
      this.tabPageCompiler.UseVisualStyleBackColor = true;
      // 
      // textBoxCompiler
      // 
      this.textBoxCompiler.Dock = System.Windows.Forms.DockStyle.Fill;
      this.textBoxCompiler.Location = new System.Drawing.Point(3, 3);
      this.textBoxCompiler.Multiline = true;
      this.textBoxCompiler.Name = "textBoxCompiler";
      this.textBoxCompiler.ReadOnly = true;
      this.textBoxCompiler.ScrollBars = System.Windows.Forms.ScrollBars.Both;
      this.textBoxCompiler.Size = new System.Drawing.Size(330, 384);
      this.textBoxCompiler.TabIndex = 0;
      // 
      // saveFileDialog
      // 
      this.saveFileDialog.Filter = "XABSL files (*.xabsl)|*.xabsl|All files|*.*";
      this.saveFileDialog.Title = "Choose name/path of the new file";
      // 
      // timerCheckForChangedFile
      // 
      this.timerCheckForChangedFile.Enabled = true;
      this.timerCheckForChangedFile.Interval = 60000;
      this.timerCheckForChangedFile.Tick += new System.EventHandler(this.timerCheckForChangedFile_Tick);
      // 
      // dropDownButtonAgent
      // 
      this.dropDownButtonAgent.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
      this.dropDownButtonAgent.Image = ((System.Drawing.Image)(resources.GetObject("dropDownButtonAgent.Image")));
      this.dropDownButtonAgent.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.dropDownButtonAgent.Name = "dropDownButtonAgent";
      this.dropDownButtonAgent.Size = new System.Drawing.Size(147, 22);
      this.dropDownButtonAgent.Text = "(agent.xabsl and symbols)";
      // 
      // MainForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(720, 488);
      this.Controls.Add(this.splitContainerMain);
      this.Controls.Add(this.toolStripMain);
      this.Controls.Add(this.statusStripMain);
      this.Controls.Add(this.menuStripMain);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MainMenuStrip = this.menuStripMain;
      this.Name = "MainForm";
      this.Text = "XabslEditor";
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
      this.Load += new System.EventHandler(this.MainForm_Load);
      this.statusStripMain.ResumeLayout(false);
      this.statusStripMain.PerformLayout();
      this.toolStripMain.ResumeLayout(false);
      this.toolStripMain.PerformLayout();
      this.menuStripMain.ResumeLayout(false);
      this.menuStripMain.PerformLayout();
      this.splitContainerMain.Panel1.ResumeLayout(false);
      this.splitContainerMain.Panel2.ResumeLayout(false);
      this.splitContainerMain.ResumeLayout(false);
      this.tabControlMain.ResumeLayout(false);
      this.tabPageOption.ResumeLayout(false);
      this.tabPageAgent.ResumeLayout(false);
      this.tabPageErrors.ResumeLayout(false);
      this.tabPageCompiler.ResumeLayout(false);
      this.tabPageCompiler.PerformLayout();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.OpenFileDialog openFileDialog;
    private System.Windows.Forms.StatusStrip statusStripMain;
    private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel;
    private System.Windows.Forms.ToolStrip toolStripMain;
    private System.Windows.Forms.MenuStrip menuStripMain;
    private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
    private System.Windows.Forms.SplitContainer splitContainerMain;
    private System.Windows.Forms.ToolStripDropDownButton dropDownButtonOption;
    private System.Windows.Forms.ToolStripButton buttonUpdate;
    private System.Windows.Forms.ToolStripButton buttonSave;
    private System.Windows.Forms.ToolStripButton buttonOpen;
    private System.Windows.Forms.ToolStripButton buttonUndo;
    private System.Windows.Forms.ToolStripButton buttonRedo;
    private System.Windows.Forms.ToolStripMenuItem openBehaviorToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem openSingleFileToolStripMenuItem;
    private System.Windows.Forms.ToolStripButton buttonOpenBehavior;
    private System.Windows.Forms.ToolStripMenuItem editToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem undoToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem redoToolStripMenuItem;
    private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
    private System.Windows.Forms.ToolStripMenuItem autocompletitionToolStripMenuItem;
    private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
    private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem saveCurrentFileToolStripMenuItem;
    private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog1;
    private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
    private System.Windows.Forms.ToolStripMenuItem configurationToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem recentBehaviorsToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem findToolStripMenuItem;
    private System.Windows.Forms.ToolStripButton toolStripButtonFind;
    private System.Windows.Forms.ToolStripButton buttonNewFile;
    private System.Windows.Forms.ToolStripMenuItem newSingleFileToolStripMenuItem;
    private System.Windows.Forms.SaveFileDialog saveFileDialog;
    private System.Windows.Forms.ToolStripMenuItem commentCodeOutToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem uncommentCodeToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem dummyOptionToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem dummyTextToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem dummyViewToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem viewToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem updateViewToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem oneOptionBackToolStripMenuItem;
    private System.Windows.Forms.ToolStripButton buttonBack;
    private System.Windows.Forms.ToolStripButton buttonForward;
    private System.Windows.Forms.ToolStripMenuItem oneOptionForwardToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem recentOptionsToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem dummyRecentOptionToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem insertStateBodyToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem clearCacheToolStripMenuItem;
    private System.Windows.Forms.TabControl tabControlEditor;
    private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
    private System.Windows.Forms.ToolStripMenuItem newTabToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem closeTabToolStripMenuItem;
    private System.Windows.Forms.ToolStripSeparator toolStripSeparator5;
    private System.Windows.Forms.ToolStripSeparator toolStripSeparator6;
    private System.Windows.Forms.Timer timerCheckForChangedFile;
    private System.Windows.Forms.ToolStripButton buttonSearchInOptions;
    private System.Windows.Forms.ToolStripMenuItem searchInAllOptionsToolStripMenuItem;
    private System.Windows.Forms.TabControl tabControlMain;
    private System.Windows.Forms.TabPage tabPageOption;
    private System.Windows.Forms.Label labelEnableGraphView;
    private System.Windows.Forms.WebBrowser webBrowserOption;
    private System.Windows.Forms.TabPage tabPageAgent;
    private System.Windows.Forms.WebBrowser webBrowserAgent;
    private System.Windows.Forms.TabPage tabPageCompiler;
    private System.Windows.Forms.TextBox textBoxCompiler;
    private System.Windows.Forms.TabPage tabPageErrors;
    private System.Windows.Forms.TreeView treeViewErrors;
    private System.Windows.Forms.ImageList imageListMain;
    private System.Windows.Forms.ToolStripMenuItem infoToolStripMenuItem;
    private System.Windows.Forms.ToolStripDropDownButton dropDownButtonAgent;

  }
}

