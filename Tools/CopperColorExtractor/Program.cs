using System.CommandLine;
using System.Drawing;
using System.Runtime.Versioning;

[assembly: SupportedOSPlatform("windows")]

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
    int colorErrorR = 0;
    int colorErrorG = 0;
    int colorErrorB = 0;
    for (int y = 0; y < image.Height; y++)
    {
        Color pixelColor = image.GetPixel(0, y);

        // Compensate for the color error from the previous line, i.e. perform dithering
        Color compensatedPixelColor = Color.FromArgb(
            Math.Max(0, Math.Min(pixelColor.R + colorErrorR, 255)),
            Math.Max(0, Math.Min(pixelColor.G + colorErrorG, 255)),
            Math.Max(0, Math.Min(pixelColor.B + colorErrorB, 255))
        );

        var amigaColorR = compensatedPixelColor.R >> 4;
        var amigaColorG = compensatedPixelColor.G >> 4;
        var amigaColorB = compensatedPixelColor.B >> 4;
        var amigaColor = (ushort)(amigaColorR << 8 | amigaColorG << 4 | amigaColorB);
        Console.WriteLine($"Input: {pixelColor} Output: [R={amigaColorR}, G={amigaColorG}, B={amigaColorB}] Copper: {amigaColor:X4}");

        // Calculate the color error between the wanted color and the color presented on the Amiga
        colorErrorR = compensatedPixelColor.R - (amigaColorR << 4 | 0x0f);
        colorErrorG = compensatedPixelColor.G - (amigaColorG << 4 | 0x0f);
        colorErrorB = compensatedPixelColor.B - (amigaColorB << 4 | 0x0f);

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
