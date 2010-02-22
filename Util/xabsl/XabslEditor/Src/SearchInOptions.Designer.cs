namespace XabslEditor
{
  partial class SearchInOptions
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SearchInOptions));
      this.panelTop = new System.Windows.Forms.Panel();
      this.labelStatus = new System.Windows.Forms.Label();
      this.buttonClose = new System.Windows.Forms.Button();
      this.buttonFind = new System.Windows.Forms.Button();
      this.checkBoxCase = new System.Windows.Forms.CheckBox();
      this.textBoxFind = new System.Windows.Forms.TextBox();
      this.label1 = new System.Windows.Forms.Label();
      this.panelBottom = new System.Windows.Forms.Panel();
      this.listBoxResults = new System.Windows.Forms.ListBox();
      this.panelTop.SuspendLayout();
      this.panelBottom.SuspendLayout();
      this.SuspendLayout();
      // 
      // panelTop
      // 
      this.panelTop.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
      this.panelTop.Controls.Add(this.labelStatus);
      this.panelTop.Controls.Add(this.buttonClose);
      this.panelTop.Controls.Add(this.buttonFind);
      this.panelTop.Controls.Add(this.checkBoxCase);
      this.panelTop.Controls.Add(this.textBoxFind);
      this.panelTop.Controls.Add(this.label1);
      this.panelTop.Dock = System.Windows.Forms.DockStyle.Fill;
      this.panelTop.Location = new System.Drawing.Point(0, 0);
      this.panelTop.Name = "panelTop";
      this.panelTop.Size = new System.Drawing.Size(476, 276);
      this.panelTop.TabIndex = 7;
      // 
      // labelStatus
      // 
      this.labelStatus.AutoSize = true;
      this.labelStatus.Location = new System.Drawing.Point(248, 57);
      this.labelStatus.Name = "labelStatus";
      this.labelStatus.Size = new System.Drawing.Size(0, 13);
      this.labelStatus.TabIndex = 10;
      // 
      // buttonClose
      // 
      this.buttonClose.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.buttonClose.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.buttonClose.Location = new System.Drawing.Point(385, 47);
      this.buttonClose.Name = "buttonClose";
      this.buttonClose.Size = new System.Drawing.Size(75, 23);
      this.buttonClose.TabIndex = 9;
      this.buttonClose.Text = "&Close";
      this.buttonClose.UseVisualStyleBackColor = true;
      this.buttonClose.Click += new System.EventHandler(this.buttonClose_Click);
      // 
      // buttonFind
      // 
      this.buttonFind.Location = new System.Drawing.Point(15, 47);
      this.buttonFind.Name = "buttonFind";
      this.buttonFind.Size = new System.Drawing.Size(75, 23);
      this.buttonFind.TabIndex = 8;
      this.buttonFind.Text = "&Find";
      this.buttonFind.UseVisualStyleBackColor = true;
      this.buttonFind.Click += new System.EventHandler(this.buttonFind_Click);
      // 
      // checkBoxCase
      // 
      this.checkBoxCase.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.checkBoxCase.AutoSize = true;
      this.checkBoxCase.Location = new System.Drawing.Point(48, 29);
      this.checkBoxCase.Name = "checkBoxCase";
      this.checkBoxCase.Size = new System.Drawing.Size(93, 17);
      this.checkBoxCase.TabIndex = 7;
      this.checkBoxCase.Text = "case sensitive";
      this.checkBoxCase.UseVisualStyleBackColor = true;
      // 
      // textBoxFind
      // 
      this.textBoxFind.Location = new System.Drawing.Point(48, 6);
      this.textBoxFind.Name = "textBoxFind";
      this.textBoxFind.Size = new System.Drawing.Size(343, 20);
      this.textBoxFind.TabIndex = 6;
      // 
      // label1
      // 
      this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point(12, 9);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(30, 13);
      this.label1.TabIndex = 5;
      this.label1.Text = "Find:";
      // 
      // panelBottom
      // 
      this.panelBottom.Controls.Add(this.listBoxResults);
      this.panelBottom.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.panelBottom.Location = new System.Drawing.Point(0, 77);
      this.panelBottom.Name = "panelBottom";
      this.panelBottom.Size = new System.Drawing.Size(476, 199);
      this.panelBottom.TabIndex = 8;
      // 
      // listBoxResults
      // 
      this.listBoxResults.Dock = System.Windows.Forms.DockStyle.Fill;
      this.listBoxResults.FormattingEnabled = true;
      this.listBoxResults.HorizontalScrollbar = true;
      this.listBoxResults.Location = new System.Drawing.Point(0, 0);
      this.listBoxResults.Name = "listBoxResults";
      this.listBoxResults.ScrollAlwaysVisible = true;
      this.listBoxResults.Size = new System.Drawing.Size(476, 199);
      this.listBoxResults.Sorted = true;
      this.listBoxResults.TabIndex = 6;
      this.listBoxResults.DoubleClick += new System.EventHandler(this.listBoxResults_DoubleClick);
      this.listBoxResults.KeyDown += new System.Windows.Forms.KeyEventHandler(this.listBoxResults_KeyDown);
      // 
      // SearchInOptions
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.CancelButton = this.buttonClose;
      this.ClientSize = new System.Drawing.Size(476, 276);
      this.Controls.Add(this.panelBottom);
      this.Controls.Add(this.panelTop);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "SearchInOptions";
      this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Show;
      this.Text = "Search in all options";
      this.TopMost = true;
      this.panelTop.ResumeLayout(false);
      this.panelTop.PerformLayout();
      this.panelBottom.ResumeLayout(false);
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.Panel panelTop;
    private System.Windows.Forms.Label labelStatus;
    private System.Windows.Forms.Button buttonClose;
    private System.Windows.Forms.Button buttonFind;
    private System.Windows.Forms.CheckBox checkBoxCase;
    private System.Windows.Forms.TextBox textBoxFind;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Panel panelBottom;
    private System.Windows.Forms.ListBox listBoxResults;
  }
}