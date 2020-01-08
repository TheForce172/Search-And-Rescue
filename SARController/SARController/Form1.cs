using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SARController
{
    public partial class Form1 : Form
    {
        SerialPort port;
        string labelContents = "";
        public Form1()
        {
            InitializeComponent();
            List<string> data = new List<string>();
            data.Add(" ");
            comboBox1.DataSource = data.Concat(SerialPort.GetPortNames()).ToList();
            button1.Enabled = false;
            button2.Enabled = false;
        }

        private string LabelContents
        {
            get
            {
                return labelContents;
            }
            set
            {
                labelContents = value;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            port.Write("H");
        }

        private void button2_Click(object sender, EventArgs e)
        {
            port.Write("L");
        }

        private void DataReceivedHandler(
            object sender,
            SerialDataReceivedEventArgs e)
        {
            SerialPort sp = (SerialPort)sender;
            string indata = sp.ReadLine();
            LabelContents = "Data Received:" + indata;
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            port.Close();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            label1.Text = LabelContents;  // update the contents of the text box...
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            string portName = (string)comboBox1.SelectedItem;
            try
            {
                port.Close();
            }
            catch (Exception err)
            {
                labelContents = "not connected yet ";
            }
            button1.Enabled = false;
            button2.Enabled = false;
            try
            {
                port = new SerialPort(portName, 9600);
                labelContents = "connected to " + portName;
                port.Open();
                port.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);
                timer1.Interval = 100;
                timer1.Start();
                button1.Enabled = true;
                button2.Enabled = true;
            }
            catch (Exception err)
            {
                labelContents = "not connected yet ";
            }
        }
    }
}
