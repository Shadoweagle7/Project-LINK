package SE7;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.nio.channels.Pipe;
import java.util.Hashtable;
import java.nio.ByteBuffer;
import java.util.Map;
import java.util.Hashtable;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.util.Arrays;

public class Toolbox {
	private class FileUtilities {
		private File file;
		private FileInputStream fileInputStream;
		private FileOutputStream fileOutputStream;

		public FileUtilities(String path) throws FileNotFoundException {
			if (file == null) {
				throw new IllegalArgumentException("path was null");
			}

			this.file = new File(path);
			this.fileInputStream = new FileInputStream(this.file);
			this.fileOutputStream = new FileOutputStream(this.file);
		}

		public FileUtilities(File file) throws FileNotFoundException {
			if (file == null) {
				throw new IllegalArgumentException("file was null");
			}

			this.file = file;
			this.fileInputStream = new FileInputStream(this.file);
			this.fileOutputStream = new FileOutputStream(this.file);
		}

		public File getFile() {
			return this.file;
		}

		public FileInputStream getInputStream() {
			return this.fileInputStream;
		}

		public FileOutputStream getOutputStream() {
			return this.fileOutputStream;
		}
	}

	private Variant internalStream = new Variant(FileUtilities.class, Pipe.class);

	private static final int FLAG_UNSIGNED = 	0b1000000000;

	// No unsigned bool
	private static final int FLAG_BOOL = 		0b1000000001;

	private static final int FLAG_CHAR = 		0b0000000011;
	private static final int FLAG_SHORT = 		0b0000000111;

	// No unsigned wchar_t
	private static final int FLAG_WCHAR_T = 	0b1000001111;

	private static final int FLAG_INT = 		0b0000011111;
	private static final int FLAG_LONG = 		0b0000111111;
	private static final int FLAG_LONG_LONG = 	0b0001111111;

	// No unsigned float or unsigned double
	private static final int FLAG_FLOAT = 		0b101111111;
	private static final int FLAG_DOUBLE = 		0b111111111;

	private static final int SIZE_BOOL = 1;

	private static final int SIZE_CHAR = 1;
	private static final int SIZE_SHORT = 2;

	private static final int SIZE_WCHAR_T = 2;

	private static final int SIZE_INT = 4;
	private static final int SIZE_LONG = 8;
	private static final int SIZE_LONG_LONG = 16;

	private static final int SIZE_FLOAT = 4;
	private static final int SIZE_DOUBLE = 8;

	private static <K, V> K getKeyFromValueInMap(Map<K, V> map, V value) {
		int index = 0;

		for (V v : map.values()) {
			if (v.equals(value)) {
				break;
			}

			index++;
		}

		int curr = 0;

		for (K k : map.keySet()) {
			if (curr == index) {
				return k;
			}

			curr++;
		}

		return null;
	}

	private static int getTypeSize(int flag) {
		// Remove sign

		switch (flag) {
			case FLAG_BOOL:
				return SIZE_BOOL;
			case FLAG_WCHAR_T:
				return SIZE_WCHAR_T;
			case FLAG_FLOAT:
				return SIZE_FLOAT;
			case FLAG_DOUBLE:
				return SIZE_DOUBLE;
			default:
				break;
		}

		flag &= ~FLAG_UNSIGNED;

		switch (flag) {
			case FLAG_CHAR:
				return SIZE_CHAR;
			case FLAG_SHORT:
				return SIZE_SHORT;
			case FLAG_INT:
				return SIZE_INT;
			case FLAG_LONG:
				return SIZE_LONG;
			case FLAG_LONG_LONG:
				return SIZE_LONG_LONG;
			default:
				break;
		}

		return -1;
	}

	private static Hashtable<Class, java.lang.Integer> typeMapInit() {
		Hashtable<Class, java.lang.Integer> typeMapCI = new Hashtable<Class, java.lang.Integer>(14);

		typeMapCI.put(Boolean.class, FLAG_BOOL);
		typeMapCI.put(Character.class, FLAG_CHAR);
		typeMapCI.put(UnsignedCharacter.class, FLAG_UNSIGNED | FLAG_CHAR);
		typeMapCI.put(Short.class, FLAG_SHORT);
		typeMapCI.put(UnsignedShort.class, FLAG_UNSIGNED | FLAG_SHORT);
		typeMapCI.put(WideCharacter.class, FLAG_WCHAR_T);
		typeMapCI.put(Integer.class, FLAG_INT);
		typeMapCI.put(UnsignedInteger.class, FLAG_UNSIGNED | FLAG_INT);
		typeMapCI.put(Long.class, FLAG_LONG);
		typeMapCI.put(UnsignedLong.class, FLAG_UNSIGNED | FLAG_LONG);
		typeMapCI.put(LongLong.class, FLAG_LONG_LONG);
		typeMapCI.put(UnsignedLongLong.class, FLAG_UNSIGNED | FLAG_LONG_LONG);
		typeMapCI.put(Float.class, FLAG_FLOAT);
		typeMapCI.put(Double.class, FLAG_DOUBLE);

		return typeMapCI;
	}

	private static Hashtable<Class, java.lang.Integer> typeMap = typeMapInit();

	public Toolbox(File file) throws 
		VariantTypeNotFoundException, FileNotFoundException,
		IOException {
		this.internalStream.set(new FileUtilities(file));

		// From the JDK 15 documentation:
		// Returns true if the named file does not exist and was successfully created;
		// false if the named file already exists.
		file.createNewFile();
	}

	public Toolbox(Pipe pipe) throws VariantTypeNotFoundException {
		this.internalStream.set(pipe);
	}

	// Format: flag_t variable_name variable_value

	public <T> T get(String variableName) throws 
		ToolboxFileReadErrorException, IOException,
		VariantTypeNotFoundException {
		FileUtilities fileUtilties = this.internalStream.get(FileUtilities.class);
		FileInputStream i = fileUtilties.getInputStream();
		FileOutputStream o = fileUtilties.getOutputStream();

		byte[] dataTypeBytes = new byte[java.lang.Integer.BYTES];

		if (i.read(dataTypeBytes) != java.lang.Integer.BYTES) {
			// TODO: DEBUG HERE, UNABLE TO READ DATA
			System.out.println(Arrays.toString(dataTypeBytes));

			throw new ToolboxFileReadErrorException();
		}

		ByteBuffer dataTypeByteBuffer = ByteBuffer.wrap(dataTypeBytes);
		int dataTypeFlag = dataTypeByteBuffer.getInt();

		byte[] variableNameLengthBytes = new byte[java.lang.Integer.BYTES];

		if (i.read(variableNameLengthBytes) != java.lang.Integer.BYTES) {
			throw new ToolboxFileReadErrorException();
		}

		ByteBuffer variableNameLengthByteBuffer = ByteBuffer.wrap(
			variableNameLengthBytes
		);

		int variableNameLength = variableNameLengthByteBuffer.getInt();

		byte[] variableNameBytes = new byte[variableNameLength];

		if (i.read(variableNameBytes) != variableNameLength) {
			throw new ToolboxFileReadErrorException();
		}

		String variableNameInFile = new String(variableNameBytes);

		if (!variableNameInFile.equals(variableName)) {
			throw new ToolboxFileReadErrorException(
				"Name of requested variable could not be matched"
			);
		}

		Class clazz = getKeyFromValueInMap(typeMap, dataTypeFlag);

		if (clazz == null) {
			throw new ToolboxFileReadErrorException(
				"Type of requested variable could not be matched"
			);
		}

		// Get size of datatype to read, read it
		// and return it

		int dataTypeSize = getTypeSize(dataTypeFlag);

		byte[] data = new byte[dataTypeSize];

		if (i.read(data) != dataTypeSize) {
			throw new ToolboxFileReadErrorException();
		}

		System.out.println(Arrays.toString(data));

		return null;
	}

	//public <T> void set(String variableName, T value) {}

	//public <T> void create(String variableName, T value) {}

	public void erase(String variableName) {

	}
}