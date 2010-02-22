using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Xml;

namespace XabslEditor
{
  public partial class ConfigurationDlg : Form
  {

    bool _isNew;
    XmlDocument _xmlDoc;
    FileInfo _confFile;

    public XmlDocument XmlDoc 
    {
      get { return _xmlDoc; }
    }


    public ConfigurationDlg(FileInfo confFile, bool isNew)
    {
      InitializeComponent();

      _isNew = isNew;
      _confFile = confFile;

      _xmlDoc = new XmlDocument();

      if (!isNew)
      {
        // load existing properties
        try
        {
          _xmlDoc.Load(confFile.FullName);

          textBoxName.Text = _xmlDoc["prop"]["name"].InnerText;
          textBoxDot.Text = _xmlDoc["prop"]["dot"].InnerText;
          textBoxCompiler.Text = _xmlDoc["prop"]["compiler"].InnerText;
          textBoxBuild.Text = _xmlDoc["prop"]["build"].InnerText;
          textBoxApi.Text = _xmlDoc["prop"]["api"].InnerText;
          textBoxDump.Text = _xmlDoc["prop"]["dump"].InnerText;
          
        }
        catch(Exception ex)
        {
          ExceptionDialog.ShowError(ex);
        }
      }

    }

    private void buttonSave_Click(object sender, EventArgs e)
    {
      // change values
      _xmlDoc.RemoveAll();

      _xmlDoc.LoadXml(
        String.Format("<prop>\n<name>{0}</name>\n"
                      + "<dot>{1}</dot>\n"
                      + "<compiler>{2}</compiler>\n"
                      + "<api>{3}</api>\n"
                      + "<build>{4}</build>\n"
                      + "<dump>{5}</dump>\n"
                      + "</prop>",
                      textBoxName.Text,
                      textBoxDot.Text,
                      textBoxCompiler.Text,
                      textBoxApi.Text,
                      textBoxBuild.Text,
                      textBoxDump.Text));

      // write out
      try
      {
        _xmlDoc.Save(_confFile.FullName);
      }
      catch (IOException ioE)
      {
        ExceptionDialog.ShowError(ioE);
      }

      // exit
      this.Close();

    }

    private void buttonCancel_Click(object sender, EventArgs e)
    {
      this.Close();
    }

    private void buttonBrowseFile_Click(object sender, EventArgs e)
    {
      Button owner = (Button)sender;
      TextBox change = null;
      if (owner == buttonDot)
      {
        change = textBoxDot;
      }
      else if (owner == buttonCompiler)
      {
        change = textBoxCompiler;
      }
      else if (owner == buttonApi)
      {
        change = textBoxApi;
      }

      if (change != null)
      {
        openFileDialog.FileName = change.Text;
        if (openFileDialog.ShowDialog() == DialogResult.OK)
        {
          change.Text = openFileDialog.FileName;
        }
      }
    }

    private void buttonBrowseFolder_Click(object sender, EventArgs e)
    {
      Button owner = (Button)sender;
      TextBox change = null;
      if (owner == buttonBuild)
      {
        change = textBoxBuild;
      }
      else if (owner == buttonDump)
      {
        change = textBoxDump;
      }

      if (change != null)
      {
        folderBrowserDialog.SelectedPath = change.Text;
        if (folderBrowserDialog.ShowDialog() == DialogResult.OK)
        {
          change.Text = folderBrowserDialog.SelectedPath;
        }
      }
    } 
  }
}