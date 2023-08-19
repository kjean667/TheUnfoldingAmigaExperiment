using System.CommandLine;
using System.Drawing;

// Parse command line

FileInfo? inputImage = null;
FileInfo? outputFile = null;

var inputOption = new Option<FileInfo?>(
            name: "--input",
            description: "The png image file to read color information from.");

var outputOption = new Option<FileInfo?>(
            name: "--output",
            description: "The binary output file to write 16-bit words of copper colors to.");

var rootCommand = new RootCommand("Converts the first column of a png image into an Amiga color table usable for the Amiga Copper.");
rootCommand.AddOption(inputOption);
rootCommand.AddOption(outputOption);

rootCommand.SetHandler((input, output) => {
    inputImage = input;
    outputFile = output;
},
inputOption, outputOption);

await rootCommand.InvokeAsync(args);

if (inputImage == null)
{
    return;
}

// Load input image

if (inputImage.Exists)
{
    using FileStream fileStream = inputImage.OpenRead();
    FileStream? outputStream = null;
    if (outputFile != null)
    {
        outputStream = outputFile.Open(FileMode.OpenOrCreate, FileAccess.Write);
        // Overwrite any existing content in the file
        outputStream.SetLength(0);
    }

    // Convert all colors in the first column (X=0) of the input image
    using var image = new Bitmap(fileStream);
    for (int y = 0; y < image.Height; y++)
    {
        Color pixelColor = image.GetPixel(0, y);
        var amigaColorR = pixelColor.R >> 4;
        var amigaColorG = pixelColor.G >> 4;
        var amigaColorB = pixelColor.B >> 4;
        var amigaColor = (ushort)(amigaColorR << 8 | amigaColorG << 4 | amigaColorB);
        Console.WriteLine($"Input: {pixelColor} Output: [R={amigaColorR}, G={amigaColorG}, B={amigaColorB}] Copper: {amigaColor:X4}");

        // Write the Amiga color to the output file
        if (outputStream != null)
        {
            outputStream.WriteByte((byte)(amigaColor >> 8));
            outputStream.WriteByte((byte)(amigaColor & 0xff));
        }
    }
    if (outputStream != null)
    {
        outputStream.Close();
        outputStream.Dispose();
    }
}
else
{
    Console.WriteLine($"File not found: {inputImage.FullName}");
}
