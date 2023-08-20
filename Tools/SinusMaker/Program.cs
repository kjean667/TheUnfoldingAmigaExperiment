using System.CommandLine;

// Parse command line

double amplitude = 100;
int length = 64;
bool isPositive = false;
FileInfo? outputFile = null;

var amplitudeOption = new Option<double>(
            name: "--amplitude",
            getDefaultValue: () => amplitude,
            description: "Sinus height");

var lengthOption = new Option<int>(
            name: "--length",
            getDefaultValue: () => length,
            description: "Number of entries in the sinus table");

var positiveOption = new Option<bool>(
            name: "--positive",
            getDefaultValue: () => isPositive,
            description: "Flag to shift sinus to only contain positive values.");

var outputOption = new Option<FileInfo?>(
            name: "--output",
            description: "The binary output file to write 16-bit words of copper colors to.");

var rootCommand = new RootCommand("Creates a sinus table.");
rootCommand.AddOption(amplitudeOption);
rootCommand.AddOption(lengthOption);
rootCommand.AddOption(positiveOption);
rootCommand.AddOption(outputOption);

rootCommand.SetHandler((_amplitude, _length, _isPositive, _output) => {
    amplitude = _amplitude;
    length = _length;
    isPositive = _isPositive;
    outputFile = _output;
},
amplitudeOption, lengthOption, positiveOption, outputOption);

await rootCommand.InvokeAsync(args);

// Open output file if specified

FileStream? outputStream = null;
if (outputFile != null)
{
    outputStream = outputFile.Open(FileMode.OpenOrCreate, FileAccess.Write);
    // Overwrite any existing content in the file
    outputStream.SetLength(0);
}

// Create sinus

Console.WriteLine($"static const UBYTE sinus{amplitude}[] = {{");
for (int i = 0; i < length; i++)
{
    double angle = i * Math.PI * 2 / length;
    double value = Math.Sin(angle) * amplitude;
    if (isPositive)
    {
        value += amplitude;
    }
    int outputValue = (int)Math.Round(value);
    if (i > 0)
    {
        Console.Write(",");
    }
    else
    {
        Console.Write("\t");
    }
    Console.Write($"{outputValue}");

    // Write the value to the output file
    if (outputStream != null)
    {
        outputStream.WriteByte((byte)outputValue);
    }
}
Console.WriteLine("");
Console.WriteLine("};");

if (outputStream != null)
{
    outputStream.Close();
    outputStream.Dispose();
}
