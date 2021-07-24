package SE7;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.nio.ByteBuffer;
import java.util.Arrays;

public class FileToolbox extends Toolbox {
	private File file;

	private static final int FLAG_SIZE = java.lang.Integer.BYTES;

	public FileToolbox(String filePath) {
		if (filePath == null || filePath.length() == 0) {
			throw new IllegalArgumentException("Invalid filename (null or length 0)");
		}

		this.file = new File(filePath);
	}

	public void get(Primitive primitive) throws ToolboxFileReadErrorException {
		try (
			FileInputStream i = new FileInputStream(this.file)
		) {
			// Format: type variable_name_length variable_name variable_value

			byte[] endiannessByte = new byte[1];
			if (i.read(endiannessByte) != 1) {
				throw new ToolboxFileReadErrorException();
			}

			byte endianness = endiannessByte[0];

			byte[] typeFlagBytes = new byte[FLAG_SIZE];
			if (i.read(typeFlagBytes) != FLAG_SIZE) {
				throw new ToolboxFileReadErrorException();
			}

			if (endianness == Toolbox.LITTLE_ENDIAN) {
				Toolbox.endianFlip(typeFlagBytes);
			}

			int typeFlag = ByteBuffer.wrap(typeFlagBytes).getInt();

			byte[] variableNameLengthBytes = new byte[FLAG_SIZE];
			if (i.read(variableNameLengthBytes) != FLAG_SIZE) {
				throw new ToolboxFileReadErrorException();
			}

			if (endianness == Toolbox.LITTLE_ENDIAN) {
				Toolbox.endianFlip(variableNameLengthBytes);
			}

			int variableNameLength = ByteBuffer.wrap(
				variableNameLengthBytes
			).getInt();

			byte[] variableNameBytes = new byte[variableNameLength];
			if (i.read(variableNameBytes) != variableNameLength) {
				System.out.println(variableNameLength);
				throw new ToolboxFileReadErrorException();
			}

			/*
			if (endianness == Toolbox.LITTLE_ENDIAN) {
				Toolbox.endianFlip(variableNameBytes);
			}
			*/

			String variableName = new String(variableNameBytes);

			int typeSize = Toolbox.getTypeSize(typeFlag);
			if (typeFlag == -1) {
				throw new ToolboxFileReadErrorException();
			}

			byte[] typeBytes = new byte[typeSize];
			if (i.read(typeBytes) != typeSize) {
				throw new ToolboxFileReadErrorException();
			}

			if (endianness == Toolbox.LITTLE_ENDIAN) {
				Toolbox.endianFlip(typeBytes);
			}

			System.out.println(
				typeFlag + " " + variableName + " " + Arrays.toString(typeBytes)
			);
		} catch(final Exception e) {
			e.printStackTrace();
			throw new ToolboxFileReadErrorException();
		}
	}

	public void set(String variableName, Primitive value) {

	}

	public void create(String variableName, Primitive value) {

	}

	public void erase(String variableName) {

	}
}