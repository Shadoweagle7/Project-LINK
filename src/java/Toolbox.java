package src.java;

import java.io.File;
import java.nio.channels.Pipe;

public class Toolbox {
	private enum DataTransferMode {
		FILE,
		PIPE
	}

	public Toolbox(File file) {

	}

	public Toolbox(Pipe pipe) {

	}

	public <T> T get(String variableName) {

	}

	public <T> void set(String variableName, T value) {

	}

	public <T> void create(String variableName, T value) {

	}

	public void erase(String variableName) {

	}
}