using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace XabslEditor
{
  public partial class SearchInOptions : Form
  {

    private MainForm _view;
    private XabslBehavior _behavior;

    public SearchInOptions(MainForm view, XabslBehavior behavior, string initialFindText)
    {
      InitializeComponent();

      if (behavior == null)
      {
        // who the ...
        Exception ex = new Exception("passed a null-object as behavior to SearchInOptions-dialog");
        throw ex;
      }

      if (initialFindText != null)
      {
        textBoxFind.Text = initialFindText;
      }

      _view = view;
      _behavior = behavior;
    }

    private void buttonClose_Click(object sender, EventArgs e)
    {
      this.Close();
    }

    private void buttonFind_Click(object sender, EventArgs e)
    {
      labelStatus.Text = "searching...";

      listBoxResults.Items.Clear();
      bool found_something = false;
      foreach (System.IO.FileInfo info in _behavior.Options)
      {
        
        System.IO.StreamReader reader = new System.IO.StreamReader(info.FullName);
        string content = reader.ReadToEnd();
        reader.Close();

        int pos;
        if (!checkBoxCase.Checked)
        {
          pos = content.IndexOf(textBoxFind.Text, StringComparison.InvariantCultureIgnoreCase);
        }
        else
        {
          pos = content.IndexOf(textBoxFind.Text, StringComparison.InvariantCulture);
        }

        if (pos > -1)
        {
          // found
          listBoxResults.Items.Add(info.FullName);
          found_something = true;
        }

      }
      if (found_something)
      {
        labelStatus.Text = "";
      }
      else
      {
        labelStatus.Text = "nothing found";
      }
    }

    private void listBoxResults_DoubleClick(object sender, EventArgs e)
    {
      if(listBoxResults.SelectedItem.ToString().Equals("")) return;

      XabslDocument doc = new XabslDocument(listBoxResults.SelectedItem.ToString(), _behavior, null);
      _view.MakeSureDocumentIsVisible(doc);
    }

    private void listBoxResults_KeyDown(object sender, KeyEventArgs e)
    {
      if (e.KeyCode == Keys.Enter)
      {
        listBoxResults_DoubleClick(listBoxResults, new EventArgs());
      }
    }
  }
}