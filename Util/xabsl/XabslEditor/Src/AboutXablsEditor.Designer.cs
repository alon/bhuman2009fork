namespace XabslEditor
{
  partial class AboutXablsEditor
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AboutXablsEditor));
      this.btOk = new System.Windows.Forms.Button();
      this.labelProductName = new System.Windows.Forms.Label();
      this.labelVersion = new System.Windows.Forms.Label();
      this.labelCopyright = new System.Windows.Forms.Label();
      this.linkLabelXABSL = new System.Windows.Forms.LinkLabel();
      this.label1 = new System.Windows.Forms.Label();
      this.SuspendLayout();
      // 
      // btOk
      // 
      this.btOk.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.btOk.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.btOk.Location = new System.Drawing.Point(289, 97);
      this.btOk.Name = "btOk";
      this.btOk.Size = new System.Drawing.Size(75, 23);
      this.btOk.TabIndex = 0;
      this.btOk.Text = "&OK";
      this.btOk.UseVisualStyleBackColor = true;
      // 
      // labelProductName
      // 
      this.labelProductName.AutoSize = true;
      this.labelProductName.Location = new System.Drawing.Point(12, 9);
      this.labelProductName.Name = "labelProductName";
      this.labelProductName.Size = new System.Drawing.Size(94, 13);
      this.labelProductName.TabIndex = 1;
      this.labelProductName.Text = "labelProductName";
      // 
      // labelVersion
      // 
      this.labelVersion.AutoSize = true;
      this.labelVersion.Location = new System.Drawing.Point(12, 33);
      this.labelVersion.Name = "labelVersion";
      this.labelVersion.Size = new System.Drawing.Size(64, 13);
      this.labelVersion.TabIndex = 2;
      this.labelVersion.Text = "labelVersion";
      // 
      // labelCopyright
      // 
      this.labelCopyright.AutoSize = true;
      this.labelCopyright.Location = new System.Drawing.Point(12, 55);
      this.labelCopyright.Name = "labelCopyright";
      this.labelCopyright.Size = new System.Drawing.Size(73, 13);
      this.labelCopyright.TabIndex = 3;
      this.labelCopyright.Text = "labelCopyright";
      // 
      // linkLabelXABSL
      // 
      this.linkLabelXABSL.AutoSize = true;
      this.linkLabelXABSL.Location = new System.Drawing.Point(141, 77);
      this.linkLabelXABSL.Name = "linkLabelXABSL";
      this.linkLabelXABSL.Size = new System.Drawing.Size(231, 13);
      this.linkLabelXABSL.TabIndex = 6;
      this.linkLabelXABSL.TabStop = true;
      this.linkLabelXABSL.Text = "http://www2.informatik.hu-berlin.de/ki/XABSL/";
      this.linkLabelXABSL.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkLabelXABSL_LinkClicked);
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point(12, 77);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(123, 13);
      this.label1.TabIndex = 7;
      this.label1.Text = "Visit the XABSL website:";
      // 
      // AboutXablsEditor
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(376, 132);
      this.Controls.Add(this.label1);
      this.Controls.Add(this.linkLabelXABSL);
      this.Controls.Add(this.labelCopyright);
      this.Controls.Add(this.labelVersion);
      this.Controls.Add(this.labelProductName);
      this.Controls.Add(this.btOk);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "AboutXablsEditor";
      this.Padding = new System.Windows.Forms.Padding(9);
      this.ShowInTaskbar = false;
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Text = "About XablsEditor";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Button btOk;
    private System.Windows.Forms.Label labelProductName;
    private System.Windows.Forms.Label labelVersion;
    private System.Windows.Forms.Label labelCopyright;
    private System.Windows.Forms.LinkLabel linkLabelXABSL;
    private System.Windows.Forms.Label label1;




  }
}
