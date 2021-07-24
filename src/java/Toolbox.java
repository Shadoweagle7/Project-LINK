package SE7;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.nio.channels.Pipe;
import java.util.Hashtable;
import java.util.ArrayList;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.util.stream.Stream;
import java.util.stream.Collectors;

public abstract class Toolbox {
	protected static final int FLAG_UNSIGNED = 	0b1000000000;

	// No unsigned bool
	protected static final int FLAG_BOOL = 		0b1000000001;

	protected static final int FLAG_CHAR = 		0b0000000011;
	protected static final int FLAG_SHORT = 	0b0000000111;

	// No unsigned wchar_t
	protected static final int FLAG_WCHAR_T = 	0b1000001111;

	protected static final int FLAG_INT = 		0b0000011111;
	protected static final int FLAG_LONG = 		0b0000111111;
	protected static final int FLAG_LONG_LONG = 0b0001111111;

	// No unsigned float or unsigned double
	protected static final int FLAG_FLOAT = 	0b101111111;
	protected static final int FLAG_DOUBLE = 	0b111111111;

	protected static final int SIZE_BOOL = 1;

	protected static final int SIZE_CHAR = 1;
	protected static final int SIZE_SHORT = 2;

	protected static final int SIZE_WCHAR_T = 2;

	protected static final int SIZE_INT = 4;
	protected static final int SIZE_LONG = 8;
	protected static final int SIZE_LONG_LONG = 16;

	protected static final int SIZE_FLOAT = 4;
	protected static final int SIZE_DOUBLE = 8;

	protected static int getTypeSize(int flag) {
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

	// Note to self: Java is always Big - Endian

	protected static byte[] endianFlip(byte[] bytes) {
		if (bytes == null){
			return bytes;
		}

		int offset = bytes.length % 2 == 0 ? 1 : 0;
		int beginIndex = 0, endIndex = bytes.length - 1;

		while (beginIndex + offset != endIndex) {
			byte temp = bytes[beginIndex];
			bytes[beginIndex] = bytes[endIndex];
			bytes[endIndex] = temp;

			beginIndex++;
			endIndex--;
		}

		if (offset == 1) {
			byte temp = bytes[beginIndex];
			bytes[beginIndex] = bytes[endIndex];
			bytes[endIndex] = temp;
		}

		return bytes;
	}

	protected static final byte LITTLE_ENDIAN = 0;
	protected static final byte BIG_ENDIAN = 1;

	public static class TypePair {
		public Class type;
		public java.lang.Integer flag;

		public TypePair(Class type, java.lang.Integer flag) {
			this.type = type;
			this.flag = flag;
		}

		public boolean equals(Object object) {
			if (this == object) {
				return true;
			} else if (
				object == null || 
				object.getClass() != this.getClass()
			) {
				return false;	
			}

			TypePair typePair = (TypePair)object;

			return
				typePair.type != null &&
				typePair.flag != null &&
				this.flag != null &&
				this.flag != null &&
				typePair.type.equals(this.type) &&
				typePair.flag.equals(this.flag);
		}
	}

	public static class TypeMap {
		private ArrayList<TypePair> typePairs;
		private static final int defaultInitialCapacity = 14;

		public TypeMap() {
			this.typePairs = new ArrayList<TypePair>(defaultInitialCapacity);
		}

		public TypeMap(int initialCapactity) {
			this.typePairs = new ArrayList<TypePair>(initialCapactity);
		}

		void add(TypePair typePair) {
			this.typePairs.add(typePair);
		}

		Class at(java.lang.Integer flag) {
			Stream<TypePair> streamTypePairs = this.typePairs.stream().filter(
				typePair -> typePair.flag == flag
			);

			if (streamTypePairs.count() == 0) {
				throw new IllegalArgumentException("Cannot find flag");
			}

			return streamTypePairs.collect(Collectors.toList()).get(0).type;
		}

		java.lang.Integer at(Class type) {
			Stream<TypePair> streamTypePairs = this.typePairs.stream().filter(
				typePair -> typePair.type == type
			);

			if (streamTypePairs.count() == 0) {
				throw new IllegalArgumentException("Cannot find flag");
			}

			return streamTypePairs.collect(Collectors.toList()).get(0).flag;
		}
	}

	private static Lock typeMapLock = new ReentrantLock();

	private static TypeMap typeMapInit() {
		typeMapLock.lock();

		TypeMap typeMapCI = new TypeMap(14);

		typeMapCI.add(new TypePair(Boolean.class, FLAG_BOOL));
		typeMapCI.add(new TypePair(Character.class, FLAG_CHAR));
		typeMapCI.add(new TypePair(UnsignedCharacter.class, FLAG_UNSIGNED | FLAG_CHAR));
		typeMapCI.add(new TypePair(Short.class, FLAG_SHORT));
		typeMapCI.add(new TypePair(UnsignedShort.class, FLAG_UNSIGNED | FLAG_SHORT));
		typeMapCI.add(new TypePair(WideCharacter.class, FLAG_WCHAR_T));
		typeMapCI.add(new TypePair(Integer.class, FLAG_INT));
		typeMapCI.add(new TypePair(UnsignedInteger.class, FLAG_UNSIGNED | FLAG_INT));
		typeMapCI.add(new TypePair(Long.class, FLAG_LONG));
		typeMapCI.add(new TypePair(UnsignedLong.class, FLAG_UNSIGNED | FLAG_LONG));
		typeMapCI.add(new TypePair(LongLong.class, FLAG_LONG_LONG));
		typeMapCI.add(new TypePair(UnsignedLongLong.class, FLAG_UNSIGNED | FLAG_LONG_LONG));
		typeMapCI.add(new TypePair(Float.class, FLAG_FLOAT));
		typeMapCI.add(new TypePair(Double.class, FLAG_DOUBLE));

		try {
			typeMapLock.unlock();
		} catch (final Exception e) {}

		return typeMapCI;
	}

	protected static TypeMap typeMap = typeMapInit();

	public abstract void get(Primitive primitive) throws ToolboxFileReadErrorException;

	public abstract void set(String variableName, Primitive value);

	public abstract void create(String variableName, Primitive value);

	public abstract void erase(String variableName);
}