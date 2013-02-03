package ui;

import java.io.*;
import javax.swing.*;

/**
 * System transcript - the default output stream
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

class Transcript extends PrintStream {
    
    /** the output window */
    protected JTextArea view;

    public Transcript() {
	super(new ByteArrayOutputStream());
    }

    public void setView(JTextArea v) {
	view = v;
    }

    public JTextArea getView() {
	return view;
    }

    /* the print methods */
    
    public void print(boolean b) {
	view.append(String.valueOf(b));
    }

    public void print(char c) {
	view.append(String.valueOf(c));
    }

    public void print(char[] s) {
	view.append(String.valueOf(s));
    }

    public void print(double d) {
	view.append(String.valueOf(d));
    }

    public void print(float f) {
	view.append(String.valueOf(f));
    }

    public void print(int i) {
	view.append(String.valueOf(i));
    }

    public void print(long l) {
	view.append(String.valueOf(l));
    }

    public void print(Object obj) {
	view.append(String.valueOf(obj));
    }

    public void print(String s) {
	view.append(s);
    }

    public void println() {
	view.append("\n");
    }

    public void println(boolean b) {
	print(b);
	println();
    }

    public void println(char c) {
	print(c);
	println();
    }

    public void println(char[] s) {
	print(s);
	println();
    }

    public void println(double d) {
	print(d);
	println();
    }

    public void println(float f) {
	print(f);
	println();
    }

    public void println(int i) {
	print(i);
	println();
    }

    public void println(long l) {
	print(l);
	println();
    }

    public void println(Object obj) {
	print(obj);
	println();
    }

    public void println(String s) {
	print(s);
	println();
    }

    public void write(byte[] buf, int off, int len) {
	view.append(new String(buf, off, len));
    }

    public void write(int b) {
	print((char)b);
    }
}
