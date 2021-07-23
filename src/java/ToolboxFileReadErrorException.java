package SE7;

public class ToolboxFileReadErrorException extends Exception {
	public ToolboxFileReadErrorException() {
		super("Error when reading Toolbox file");
	}

	public ToolboxFileReadErrorException(String message) {
		super(message);
	}
}